# RK3399 direct use with ADS1299

Use the common Linux capture tool from `../../common`.

## Required Linux interfaces

- ADS1299 SPI bus exposed as `/dev/spidevB.C` for bring-up.
- ADS1299 DRDY exposed as a free GPIO input under `/dev/gpiochipN`.
- Correct board pinmux and I/O voltage.

Do not copy SPI/GPIO numbers from another RK3399 board; the mapping is board-specific.

## Build

```bash
cd ../../common
make
```

## First capture

```bash
sudo ../../common/ads1299_linux_capture \
  --spi /dev/spidevB.C \
  --gpiochip /dev/gpiochipN \
  --drdy-line OFFSET \
  --speed 4000000 \
  --profile test \
  --samples 1000 \
  --csv > rk3399_test.csv
```

The program resets ADS1299, reads ID, configures the test profile, waits on falling-edge DRDY events and reads one exact 27-byte frame per event.

After the internal square-wave path works, use `--profile short`; only then move to `--profile eeg250` or your board-specific configuration.
