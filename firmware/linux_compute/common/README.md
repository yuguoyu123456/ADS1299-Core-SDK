# Linux userspace ADS1299 direct capture

`ads1299_linux_capture` is a dependency-light userspace reference for Linux SBC/SoC platforms, including the Rockchip families in this repository.

It uses only standard Linux interfaces:

- `/dev/spidevB.C` through `SPI_IOC_MESSAGE`.
- `/dev/gpiochipN` through the GPIO character-device event ioctl.
- falling-edge DRDY events for sample alignment.

No wiringPi, libgpiod or board-vendor userspace library is required.

## Build

```bash
make
```

or:

```bash
cc -O2 -Wall -Wextra -std=c11 -o ads1299_linux_capture ads1299_linux_capture.c
```

## Find devices

SPI devices:

```bash
ls -l /dev/spidev*
```

GPIO chips:

```bash
ls -l /dev/gpiochip*
```

The actual SPI bus/chip-select and DRDY line offset are board/device-tree dependent. Do not guess them from the SoC name alone.

## First hardware test: ADS1299 internal test source

```bash
sudo ./ads1299_linux_capture \
  --spi /dev/spidev1.0 \
  --gpiochip /dev/gpiochip0 \
  --drdy-line 42 \
  --speed 4000000 \
  --profile test \
  --samples 1000 \
  --csv > test.csv
```

Replace all device names and the GPIO line with the values for your board.

The tool resets the ADS1299, reads the ID register, applies the selected profile, enters RDATAC, waits for DRDY, then clocks exactly 27 bytes per sample.

## Profiles

`--profile test`
: All eight channels use gain 24 and the ADS1299 internal test source. This should be your first live SPI acquisition test.

`--profile short`
: All eight channels use gain 24 with inputs internally shorted. Use it for ADC/PGA noise and digital-link validation.

`--profile eeg250`
: 250 SPS, gain 24, normal inputs, internal reference buffer and BIAS enabled. This is a starting point, not a replacement for reviewing your board's BIASREF/reference topology. If your hardware drives BIASREF externally, adjust CONFIG3 before human measurement.

`--no-configure`
: Keeps the register configuration prepared by another process/MCU and only transitions through SDATAC/RDATAC/START for capture.

## Output formats

CSV:

```bash
--csv
```

produces:

```text
sample,status,ch1,ch2,ch3,ch4,ch5,ch6,ch7,ch8
0,C00000,123,-456,...
```

Channel values are signed raw 24-bit ADC codes sign-extended to 32-bit integers.

Raw:

```bash
--raw
```

writes the exact 27 bytes from each ADS1299 conversion to stdout. This is suitable for high-throughput piping into a recorder:

```bash
sudo ./ads1299_linux_capture ... --raw > eeg.raw
```

## Permissions

For development, `sudo` is the simplest check. For deployment, use udev/group rules that grant only the required SPI and GPIO devices instead of running the acquisition service as root.

## Rockchip device-tree requirements

The board kernel/device tree must expose:

1. an enabled SPI controller;
2. a spidev child (or your production kernel driver) on the ADS1299 CS;
3. the DRDY pin as a GPIO line not claimed by another driver;
4. matching I/O voltage and pinmux configuration.

If `/dev/spidev*` is missing, userspace code cannot fix the device tree. Enable the SPI controller/pinmux first.

## Production notes

For a final medical/research acquisition appliance, consider moving the DRDY + SPI path into a kernel/IIO driver or FPGA when deterministic latency and multi-device synchronization are required. The userspace tool is intentionally transparent and useful for bring-up, validation, logging and reference behavior.
