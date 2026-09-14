# RK3566 ADS1299 Linux Quick Start

Validation status: **TEMPLATE / HOST-COMPILE TARGET** until the dedicated CI for this leaf passes. No physical RK3566 board or ADS1299 hardware is claimed as BOARD-VERIFIED.

## 1. Board/BSP prerequisite

Use an RK3566 Linux board whose BSP exposes the selected SPI controller as `/dev/spidevX.Y` and the required GPIO bank as `/dev/gpiochipN`. The exact carrier-board header pins and GPIO line offsets vary by board/BSP, so this folder intentionally does not invent pin numbers.

## 2. Edit one file

Edit only `board_config.h` for first bring-up. Set the real `spidev` node, `gpiochip` node and CS GPIO line. RESET, PWDN, START and DRDY may remain `UINT_MAX` until wired; `UINT_MAX` means unavailable, not GPIO 0.

The Linux port configures SPI Mode 1, 8-bit words and software-controlled CS. ADS1299 register/model behavior remains in `firmware/core_driver/ads1299`.

## 3. Build

```sh
cd firmware/linux_compute/Rockchip/RK3566
make clean check
```

Expected software-only output includes:

```text
RK3566 Linux userspace compile OK: build/rk3566_ads1299_probe
Hardware probe is intentionally not executed by host validation.
```

## 4. Run the hardware probe

After the BSP exposes the configured device nodes and the process has permission to access them:

```sh
./build/rk3566_ads1299_probe
```

A correctly connected ADS1299-family device should produce a line beginning with:

```text
ADS1299 probe OK: raw_id=...
```

Failures are separated into CONFIG, PORT, CORE, SPI/ID and unexpected-ID classes.

## 5. Wiring contract

Connect SCLK, MOSI/DIN and MISO/DOUT to the SPI controller selected by `RK3566_ADS1299_SPIDEV`. CS is intentionally a GPIO because the portable core owns transaction boundaries. Use a common digital ground and voltage levels compatible with the actual ADS1299 board. Resolve RESET, PWDN, START and DRDY line offsets from the exact carrier/BSP rather than Linux GPIO names guessed from another board.

## Current completeness

Implemented here: one board/config boundary, Linux spidev/GPIO HAL, shared-core binding, ID/probe executable, strict host build and explicit diagnostics.

Not yet candidate-complete: named internal-test, input-short and 250-SPS EEG runnable flows; bounded DRDY streaming; multi-device example; and a physically verified RK3566 reference-board wiring table. Those should be added without moving ADS1299 register logic into this platform folder.
