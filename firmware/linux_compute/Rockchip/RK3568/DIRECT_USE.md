# RK3568 direct use with ADS1299

Use `../../common/ads1299_linux_capture.c` as the runnable reference.

## Prerequisites

Your RK3568 board device tree must enable the SPI controller connected to ADS1299, assign the correct pinmux, and leave DRDY available as a GPIO input. During bring-up, expose the ADS1299 chip select through `/dev/spidevB.C`.

Check nodes:

```bash
ls -l /dev/spidev* /dev/gpiochip*
```

The bus number, chip select and GPIO offset are board-specific.

## Build

```bash
cd ../../common
make
```

## Internal-test acquisition

```bash
sudo ./ads1299_linux_capture \
  --spi /dev/spidevB.C \
  --gpiochip /dev/gpiochipN \
  --drdy-line OFFSET \
  --speed 4000000 \
  --profile test \
  --samples 1000 \
  --csv > rk3568_test.csv
```

The tool waits on DRDY falling edges and performs one 27-byte SPI transfer per sample. Once this is stable, run the input-short profile and then the real-input configuration appropriate for your ADS1299 board.
