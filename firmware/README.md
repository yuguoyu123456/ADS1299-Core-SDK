# Firmware

All embedded acquisition code lives here.

## Simple rule

- `core_driver/` — controller-independent ADS1299 code shared by every platform.
- `mcu/` — complete reference projects organized by MCU vendor and board/MCU.
- `fpga/` — FPGA RTL, board projects and testbenches.
- `common/` — shared embedded utilities such as the packet encoder.

A user should not need to assemble a project from separate `ports/`, `examples/` and `projects/` folders. Platform-specific adaptation belongs inside the corresponding complete MCU project.

## Platform quality levels

- **Planned** — roadmap only.
- **Compiles** — clean toolchain build verified.
- **Bench-tested** — real ADS1299 hardware verified.
- **24h-tested** — long-duration acquisition verified.

Only complete projects belong in the visible MCU tree. Partial porting experiments should remain on development branches or issues until they become usable reference projects.
