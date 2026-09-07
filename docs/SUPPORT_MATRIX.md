# Platform support matrix

The machine-readable source of truth is
[`firmware/mcu/catalog.json`](../firmware/mcu/catalog.json). Directory presence,
compilation and hardware validation are separate claims.

## Compile-evidenced targets

| Vendor | Target | Toolchain evidence | Hardware state |
|---|---|---|---|
| STMicroelectronics | `01_STMicroelectronics/STM32F407` | STM32CubeF4 / PlatformIO | not bench-tested |
| Espressif | `02_Espressif/ESP32S3` | ESP-IDF CI history | not bench-tested |
| Nordic | `07_Nordic/nRF52840` | NCS v3.4.0 LTS CI history | not bench-tested |
| Nordic | `07_Nordic/nRF5340` | NCS v3.4.0 LTS CI history | not bench-tested |
| Raspberry Pi | `10_RaspberryPi/RP2040` | Pico SDK 2.3.0 CI history | not bench-tested |
| Raspberry Pi | `10_RaspberryPi/RP2350` | Pico SDK 2.3.0 CI history | not bench-tested |
| GigaDevice | `11_GigaDevice/GD32F450` | pinned PlatformIO/SPL | not bench-tested |
| WCH | `12_WCH/CH32V307` | pinned PlatformIO/NoneOS | not bench-tested |

In addition, 66 targets are `Reference` and 11 are `Planned`. Their maintained
port contract, documentation and tests exist, but no clean vendor-toolchain
build is claimed. The unified rank source is
[`firmware/ecosystem_catalog.json`](../firmware/ecosystem_catalog.json).

## Status vocabulary

Only these values may appear: `Planned`, `Reference`, `Example`, `Compatible`,
`Compiles`, `Bench-tested`, `24h-tested`.

- `Compiles` requires an exact SDK/compiler version and reproducible clean log.
- `Bench-tested` requires real ADS1299 ID, internal-test and frame evidence.
- `24h-tested` additionally requires continuous loss/CRC/error statistics.

Run `python tests/validate_mcu_catalog.py` to check structure, status consistency
and the rule that Port source contains no ADS1299 register/configuration logic.

## FPGA reference packages (ranks 71-85)

| Vendor | Reference family | Constraint format | Current state |
|---|---|---|---|
| AMD Xilinx | Zynq-7000, Zynq UltraScale+, Artix-7, Kintex-7 | XDC | Reference |
| Intel FPGA | Cyclone V, Cyclone IV, MAX 10 | QSF | Reference |
| Lattice | ECP5, Certus-NX | LPF | Reference |
| Microchip FPGA | PolarFire | PDC | Reference |
| Gowin / Tang | GW1N, GW2A, Tang GW series | CST | Reference |
| Anlogic | EG4 | ADC | Reference |
| Efinix | Titanium | SDC | Reference |

All 15 packages share the same vendor-neutral RTL and self-checking
testbench. Run `python tests/validate_fpga_ecosystem.py` to detect RTL drift,
missing files, generated artifacts and register-policy leakage.
