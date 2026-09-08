# RK3588 direct use with ADS1299

Use the shared Linux capture implementation from `../../common`.

## Bring-up prerequisites

The RK3588 board device tree must expose the SPI controller and DRDY GPIO actually wired to the ADS1299 connector. Board vendors often route different SPI controllers to different headers, so determine values from the specific board schematic/device tree rather than assuming a universal RK3588 pinout.

```bash
ls -l /dev/spidev* /dev/gpiochip*
```

## Build

```bash
cd ../../common
make
```

## Test profile

```bash
sudo ./ads1299_linux_capture \
  --spi /dev/spidevB.C \
  --gpiochip /dev/gpiochipN \
  --drdy-line OFFSET \
  --speed 4000000 \
  --profile test \
  --samples 2000 \
  --csv > rk3588_test.csv
```

The reference uses GPIO falling-edge events to align each 27-byte SPI read to DRDY. For long-duration/high-channel-count acquisition, RK3588 can also receive already-framed data from the FPGA transport layer; that architecture is preferable when eight ADS1299 devices must remain tightly synchronized.

Validation order: ID read -> internal test -> shorted input -> real signal.
