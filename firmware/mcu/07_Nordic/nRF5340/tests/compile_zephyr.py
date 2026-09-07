"""Clean hardware-SPI application build using external, pinned Zephyr modules."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile

p = argparse.ArgumentParser(description=__doc__)
for name in ('zephyr', 'cmsis', 'nordic'):
    p.add_argument('--' + name, type=Path, required=True)
for name in ('cc', 'cmake', 'ninja'):
    p.add_argument('--' + name, required=True)
a = p.parse_args()
target = Path(__file__).resolve().parents[1]
if a.cmsis.name != 'cmsis':
    p.error('Zephyr 3.7 CMSIS module directory must be named cmsis')
suffix = '.exe' if Path(a.cc).suffix == '.exe' else ''
compiler = Path(a.cc).resolve()
prefix = str(compiler.parent / 'arm-none-eabi-').replace('\\', '/')
env = {**os.environ, 'ZEPHYR_BASE': a.zephyr.resolve().as_posix(),
       'ZEPHYR_TOOLCHAIN_VARIANT': 'cross-compile', 'CROSS_COMPILE': prefix}
with tempfile.TemporaryDirectory(prefix='.verify-', dir=target / 'tests') as td:
    build = Path(td)
    subprocess.run([a.cmake, '-S', str(target), '-B', str(build), '-G', 'Ninja',
        '-DBOARD=nrf5340dk/nrf5340/cpuapp', f'-DCMAKE_MAKE_PROGRAM={a.ninja}',
        f'-DPython3_EXECUTABLE={Path(sys.executable).as_posix()}',
        f'-DUSER_CACHE_DIR={(build / "cache").as_posix()}',
        f'-DZEPHYR_MODULES={a.cmsis.resolve().as_posix()};{a.nordic.resolve().as_posix()}'],
        env=env, check=True)
    subprocess.run([a.cmake, '--build', str(build), '--parallel', '4'], env=env, check=True)
    elf = build / 'zephyr/zephyr.elf'
    config = (build / 'zephyr/.config').read_text(encoding='utf-8')
    for expected in ('CONFIG_SPI=y', 'CONFIG_NRFX_SPIM4=y'):
        if expected not in config.splitlines():
            raise RuntimeError(f'Hardware SPI configuration missing: {expected}')
    nm = str(compiler.parent / ('arm-none-eabi-nm' + suffix))
    undefined = subprocess.check_output([nm, '-u', str(elf)], text=True)
    if undefined.strip():
        raise RuntimeError(f'Undefined ELF symbols: {undefined}')
    evidence = {'result': 'clean nRF5340 hardware SPIM4 ELF build passed',
        'scope': 'application core only; no hardware execution, BLE or acquisition test',
        'undefined_symbols': 0, 'sha256': hashlib.sha256(elf.read_bytes()).hexdigest(),
        'size': subprocess.check_output([str(compiler.parent / ('arm-none-eabi-size' + suffix)),
            str(elf)], text=True).splitlines()[1].split()[:4]}
print(json.dumps(evidence, indent=2))
