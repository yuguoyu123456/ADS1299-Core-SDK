# Firmware

Host-link communication is isolated in [`transport/`](transport/). MCU ports
only adapt SPI, GPIO, DRDY and timing; they must not own UART, USB, BLE, Wi-Fi,
Ethernet or LSL protocol policy.

All embedded acquisition code lives here.

## Three-layer rule

- `core_driver/` — controller-independent ADS1299 code shared by every platform.
- `mcu/` — thin MCU hardware ports, minimum examples and integration records.
- `fpga/` — vendor-neutral RTL interfaces, constraint templates and testbenches.
- `common/` — shared embedded utilities such as the packet encoder.
- `transport/` — callback-based UART/USB/BLE/network transport boundary.

ADS1299 register, command, field and configuration behavior must exist only in
`core_driver/ads1299`. MCU ports own SPI/GPIO/DRDY/delay callbacks; FPGA RTL
transports opaque bytes and parses frames. Examples only demonstrate the call
sequence. Vendor SDKs, startup files, linker scripts and generated middleware
remain external.

## Platform quality levels

- **Planned** — roadmap only.
- **Reference / Example / Compatible** — increasing integration maturity with
  no implied clean build.
- **Compiles** — a documented clean toolchain build exists.
- **Bench-tested** — real ADS1299 hardware verified.
- **24h-tested** — long-duration acquisition verified.

Every visible target must have complete maintenance metadata and an honest
status. A `Reference` port template is allowed; an empty directory or invented
hardware/build result is not.

## Maintainer checks

```bash
python tests/validate_mcu_catalog.py
python tests/validate_fpga_ecosystem.py
```

Cross-platform edits must be made in the generators under `tools/` and then
regenerated, preventing 85 MCU/SoC/DSP targets or 15 FPGA packages from drifting.
The continuous rank index is `ecosystem_catalog.json` / `ECOSYSTEM_100.md`.
# Linux 算力板扩展入口

[linux_compute/](linux_compute/README.md) 独立维护 Rockchip 等 Linux 算力板的
ADS1299 接入规划，与 mcu/、fpga/ 并列。当前六个 SoC 候选均为 Planned，
不计入原有 1–100 排名或已验证支持数量。
