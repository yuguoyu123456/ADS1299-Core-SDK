# ADS1299 MCU Port Ecosystem

第 101–200 项已建立为独立的 Planned 扩展清单：
[查看 100 个新增 MCU 目录](ECOSYSTEM_101_200.md)。
机器可读元信息在 `catalog_101_200.json`。这些是后续开发候选，
编号不表示全球销量排名，通用模板尚未绑定各平台官方 SDK。
本树包含原清单的 85 个目标和新增 100 个 Planned 目录，共 185 个目标目录。

Linux compute-board planning is maintained separately in
[Linux compute boards](../linux_compute/README.md), initially covering
six Rockchip SoC families. These Planned extensions are outside ranks 1–100
and are not included in the 85 ranked packages or validated-support counts.

This tree is the hardware-adaptation layer between the controller-independent
ADS1299 Core and a user's official MCU SDK project. It is not a vendor-SDK
mirror and does not contain product firmware.

## Three-layer boundary

1. `firmware/core_driver/ads1299/` owns commands, registers, fields,
   configuration, channel/BIAS/SRB/lead-off behavior, frames and conversion.
2. Each MCU `ads1299_port/` owns only SPI, GPIO, CS, DRDY, RESET, PWDN, START
   and delay callbacks. Host transports remain outside the ADS1299 logic.
3. Each `examples/main_ads1299.c` shows only the minimum Core call sequence.

Port source must never define or rewrite ADS1299 register bits. The structural
validator enforces this rule.

## Ranked coverage and quality tiers

- Ranks 1-20: Tier A core platforms; prioritize complete ports, examples, tests and capability analysis.
- Ranks 21-60: Tier B important ecosystems; maintain ports, examples and integration documentation.
- Ranks 61-70 and 86-100: Tier C extensions; maintain honest templates and architecture notes.
- Ranks 71-85 are FPGA/CPLD and therefore live under `firmware/fpga/`.

This directory contains 85 maintained MCU/SoC/DSP packages. Concrete but
uncompiled ports are `Reference`; entries without a sufficiently identified
public SDK/device remain `Planned`.

Every maintained MCU target contains:

```text
README.md                  platform_info.yaml
capability.md              integration.md
version.md                 validation.md
ads1299_port/              examples/main_ads1299.c
board/pinmap.md            tests/
```

## Vendor SDK policy

Do not commit full SDKs, CMSIS/HAL trees, startup code, linker scripts, IDE
caches or middleware. Install the official SDK externally, add Core plus the
small target port, and implement `board_ads1299_hal()` with that SDK. Reference
sources and licenses must be recorded before importing any third-party code.

## Validation

Allowed states are `Planned`, `Reference`, `Example`, `Compatible`, `Compiles`,
`Bench-tested` and `24h-tested`. No state implies a higher one.

```bash
python tests/validate_mcu_catalog.py
```

The generated maintenance packages are reproducible with
`python tools/maintain_ads1299_ecosystem.py`; edit that source of truth when a
cross-platform schema change is required.
