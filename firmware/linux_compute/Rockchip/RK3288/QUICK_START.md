# RK3288 Linux ADS1299 Quick Start

Validation status: **TEMPLATE / HOST-COMPILE-VERIFIED when the dedicated CI passes.**
This does not imply an RK3288 board, electrical, timing, or ADS1299 hardware test.

This leaf now contains a real Linux userspace port using the kernel `spidev`
interface and GPIO character-device ABI. ADS1299 command/register behavior stays
in `firmware/core_driver/ads1299`; this folder only supplies Linux I/O.

## 1. Board/BSP prerequisite

Use an RK3288 Linux board/BSP that exposes the selected SPI controller as a
`/dev/spidevX.Y` node and exposes the required GPIO controller as
`/dev/gpiochipN`. The repository intentionally does not guess a carrier-board
header pinout or GPIO line number.

Before wiring, verify the exact board schematic/device tree and 3.3-V logic
compatibility. Confirm Linux-visible resources with commands such as:

```sh
ls -l /dev/spidev* /dev/gpiochip*
gpioinfo   # when the distribution provides libgpiod tools
```

## 2. Edit one file

Edit only `board_config.h` for first bring-up:

- `RK3288_ADS1299_SPIDEV`
- `RK3288_ADS1299_GPIOCHIP`
- `RK3288_ADS1299_CS_LINE` (required)
- RESET/PWDN/START/DRDY line offsets when those signals are wired

GPIO values are Linux gpiochip **line offsets**, not raw RK3288 bank/pin
numbers. Unresolved lines remain `UINT_MAX`; the probe refuses to run with an
unconfigured CS instead of touching an arbitrary GPIO.

The port configures SPI Mode 1, 8 bits per word, MSB-first through Linux spidev
and disables the controller's automatic chip select so the shared ADS1299 core
can own CS timing through its normal `ads1299_port_t` callback.

## 3. Build

From this directory:

```sh
make clean check
```

Expected software-only result:

```text
RK3288 Linux userspace compile OK: build/rk3288_ads1299_probe
Hardware probe is intentionally not executed by host CI.
```

## 4. Run the ID probe on the target

After configuring the real device nodes/line offsets and granting the process
access to them:

```sh
sudo ./build/rk3288_ads1299_probe
```

A correctly communicating ADS1299-family device prints a line similar to:

```text
ADS1299 probe OK: raw_id=0x.. family=1 channels=8 revision=..
```

Failures are separated into CONFIG, PORT, CORE, SPI/ID, and unexpected-ID
classes so first bring-up does not require editing the shared driver.

## Current scope

This pass provides the missing configuration layer, Linux SPI/GPIO port,
build entrypoint, and executable probe. Internal-test, input-short, EEG 250 SPS,
continuous streaming, DRDY buffering and an exact reference-board wiring table
remain required before this leaf can be considered candidate-complete for the
zero-beginner gate.
