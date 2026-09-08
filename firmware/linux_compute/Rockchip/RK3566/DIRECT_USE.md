# RK3566 direct use with ADS1299

The runnable reference is `../../common/ads1299_linux_capture.c`.

## Board preparation

Enable the SPI controller and pinmux used by the ADS1299 connector. For userspace bring-up, expose the selected chip select as `/dev/spidevB.C`. Leave DRDY available as a GPIO character-device line.

Find actual device nodes on the running board:

```bash
ls -l /dev/spidev* /dev/gpiochip*
```

GPIO line offsets must come from the board device tree/schematic or GPIO inspection tools; they are not fixed by RK3566 itself.

## Build and test

```bash
cd ../../common
make
sudo ./ads1299_linux_capture \
  --spi /dev/spidevB.C \
  --gpiochip /dev/gpiochipN \
  --drdy-line OFFSET \
  --profile test \
  --samples 1000 \
  --csv > rk3566_test.csv
```

Confirm the ADS1299 ID message appears on stderr and that samples arrive at the expected rate. Continue with `--profile short` before normal input.
