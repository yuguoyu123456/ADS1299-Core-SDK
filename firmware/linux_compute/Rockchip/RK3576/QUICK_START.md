# RK3576 ADS1299 Linux Quick Start

Validation status: **TEMPLATE / HOST-COMPILE TARGET** until the dedicated CI for this leaf passes. No physical RK3576 board or ADS1299 hardware is claimed as BOARD-VERIFIED.

## Board/BSP prerequisite
Use an RK3576 Linux board whose BSP exposes the selected SPI controller as `/dev/spidevX.Y` and the required GPIO bank as `/dev/gpiochipN`. Exact carrier header pins and GPIO line offsets vary by board/BSP, so this folder intentionally does not invent pin numbers.

## Edit one file
Edit only `board_config.h` for first bring-up. Set the real spidev node, gpiochip node and CS GPIO line. RESET, PWDN, START and DRDY may remain `UINT_MAX` until wired. The port configures SPI Mode 1, 8-bit words and software-controlled CS; ADS1299 register/model behavior remains in `firmware/core_driver/ads1299`.

## Build
```sh
cd firmware/linux_compute/Rockchip/RK3576
make clean check
```
Expected software-only output includes `RK3576 Linux userspace compile OK`. Host validation intentionally does not access hardware.

## Run probe
After device-tree exposure and permissions are correct:
```sh
./build/rk3576_ads1299_probe
```
A correctly connected ADS1299-family device should print `ADS1299 probe OK: raw_id=...`. Failures are separated into CONFIG, PORT, CORE, SPI/ID and unexpected-ID classes.

## Wiring contract
Connect SCLK, MOSI/DIN and MISO/DOUT to the SPI controller selected by `RK3576_ADS1299_SPIDEV`. CS is a GPIO because the portable core owns transaction boundaries. Use common digital ground and voltage levels compatible with the actual ADS1299 board. Resolve RESET, PWDN, START and DRDY line offsets from the exact carrier/BSP.

## Current completeness
Implemented: one board/config boundary, Linux spidev/GPIO HAL, shared-core binding, ID/probe executable, strict host build and explicit diagnostics.

Not yet candidate-complete: named internal-test, input-short and 250-SPS EEG runnable flows; bounded DRDY streaming; multi-device example; and a physically verified RK3576 reference-board wiring table. Add those without moving ADS1299 register logic into this platform folder.
