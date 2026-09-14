#!/usr/bin/env python3
"""Compile and run every standardized FPGA ADS1299 reference testbench.

This runner intentionally keeps the FPGA package contract explicit and independent
of vendor synthesis tools. It validates the portable reference RTL with Icarus
Verilog and reports the exact package that fails.

Validation level: host simulation only. A passing result does not imply vendor
synthesis, timing closure, or physical-board verification.
"""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
FPGA_ROOT = ROOT / "firmware" / "fpga"
REQUIRED_RELATIVE_FILES = (
    Path("rtl/ads1299_spi_master.v"),
    Path("rtl/ads1299_controller.v"),
    Path("rtl/frame_parser.v"),
    Path("tb/simulation_tb.v"),
)


def fail(message: str) -> int:
    print(f"ERROR: {message}", file=sys.stderr)
    return 1


def main() -> int:
    iverilog = shutil.which("iverilog")
    vvp = shutil.which("vvp")
    if not iverilog or not vvp:
        return fail("iverilog and vvp must both be installed and available on PATH")

    manifests = sorted(FPGA_ROOT.rglob("project.json"))
    if not manifests:
        return fail(f"no FPGA project.json manifests found under {FPGA_ROOT}")

    print(f"FPGA reference simulation: {len(manifests)} package(s)")

    with tempfile.TemporaryDirectory(prefix="ads1299-fpga-") as temp_dir:
        temp_root = Path(temp_dir)

        for index, manifest in enumerate(manifests, start=1):
            package = manifest.parent
            package_name = package.name

            try:
                metadata = json.loads(manifest.read_text(encoding="utf-8"))
            except (OSError, json.JSONDecodeError) as exc:
                return fail(f"{manifest.relative_to(ROOT)}: invalid manifest: {exc}")

            missing = [rel for rel in REQUIRED_RELATIVE_FILES if not (package / rel).is_file()]
            if missing:
                missing_text = ", ".join(str(rel) for rel in missing)
                return fail(f"{package.relative_to(ROOT)}: missing required file(s): {missing_text}")

            display_name = metadata.get("name") or package_name
            output = temp_root / f"{index:02d}_{package_name}.vvp"
            sources = [str(package / rel) for rel in REQUIRED_RELATIVE_FILES]

            print(f"[{index:02d}/{len(manifests):02d}] {display_name}: compile")
            compile_result = subprocess.run(
                [iverilog, "-g2012", "-o", str(output), *sources],
                cwd=ROOT,
                text=True,
                capture_output=True,
                check=False,
            )
            if compile_result.returncode != 0:
                if compile_result.stdout:
                    print(compile_result.stdout, end="")
                if compile_result.stderr:
                    print(compile_result.stderr, end="", file=sys.stderr)
                return fail(
                    f"{package.relative_to(ROOT)}: Icarus Verilog compile failed "
                    f"with exit code {compile_result.returncode}"
                )

            print(f"[{index:02d}/{len(manifests):02d}] {display_name}: simulate")
            sim_result = subprocess.run(
                [vvp, str(output)],
                cwd=ROOT,
                text=True,
                capture_output=True,
                check=False,
            )
            if sim_result.stdout:
                print(sim_result.stdout, end="")
            if sim_result.stderr:
                print(sim_result.stderr, end="", file=sys.stderr)
            if sim_result.returncode != 0:
                return fail(
                    f"{package.relative_to(ROOT)}: simulation failed "
                    f"with exit code {sim_result.returncode}"
                )

    print(f"FPGA reference simulation PASS: {len(manifests)} package(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
