# RK3576 direct use with ADS1299

The common Linux acquisition program lives in `../../common` and can be built directly on the target system or cross-compiled.

## Device-tree checklist

1. Enable the SPI controller connected to ADS1299.
2. Configure MOSI/MISO/SCLK/CS pinmux.
3. Expose a spidev node for bring-up or replace it later with a production driver.
4. Configure ADS1299 DRDY as an unclaimed GPIO input.
5. Verify board I/O voltage compatibility.

Do not hard-code SPI or GPIO identifiers from another RK3576 board.

## Build and run

```bash
cd ../../common
make
sudo ./ads1299_linux_capture \
  --spi /dev/spidevB.C \
  --gpiochip /dev/gpiochipN \
  --drdy-line OFFSET \
  --profile test \
  --samples 1000 \
  --csv > rk3576_test.csv
```

If the ADS1299 ID read succeeds and the internal test waveform is stable, continue with the input-short profile. Use the EEG profile only after reviewing your actual reference/BIAS topology.
