# RK3288 direct use with ADS1299

Use the shared Linux userspace program in `../../common/ads1299_linux_capture.c`.

## 1. Kernel/device-tree prerequisites

Enable the SPI controller wired to ADS1299 and ensure a `/dev/spidevB.C` node is exposed for bring-up. Configure ADS1299 DRDY as a GPIO input and confirm the corresponding `/dev/gpiochipN` + line offset.

The RK3288 SoC name does **not** determine the connector pin, SPI bus number or GPIO line offset; those depend on the actual board schematic and device tree.

## 2. Build

```bash
cd ../../common
make
```

RK3288 is commonly used with 32-bit ARM Linux; the program uses standard Linux UAPI and does not depend on CPU word size.

## 3. Validate with internal test signal

```bash
sudo ../../common/ads1299_linux_capture \
  --spi /dev/spidevB.C \
  --gpiochip /dev/gpiochipN \
  --drdy-line OFFSET \
  --speed 4000000 \
  --profile test \
  --samples 1000 \
  --csv > rk3288_test.csv
```

Replace `B.C`, `N`, and `OFFSET` with the board values.

## 4. Bring-up order

Internal test -> input short -> normal EEG input. Do not start with electrodes before SPI ID, DRDY pacing and channel-code stability are verified.
