# ADS1299-Core bring-up sequence

This document is for engineering validation of a new board revision. It is not a substitute for the current TI ADS1299 datasheet.

## 1. Before connecting a person

1. Measure AVDD/DVDD and reference rails.
2. Confirm the selected clock source.
3. Verify RESET, START, DRDY and CS levels.
4. Verify SPI mode and timing with a logic analyzer.
5. Read the ID register.
6. Read/write a harmless configuration register and read it back.

## 2. Internal calibration/test signal

`ads1299_configure_internal_test()` writes CONFIG2 for the internally-generated calibration signal and switches all eight CHnSET multiplexers to TEST.

TI CONFIG2 facts used by the helper:

- bits 7:5: reserved value `110`
- bit 4: `INT_CAL=1` selects the internal test source
- bit 2: calibration amplitude selection
- bits 1:0: calibration frequency (`00=fCLK/2^21`, `01=fCLK/2^20`, `10=do not use`, `11=dc`)
- CHnSET MUX `101` selects the test signal

Start with 250 SPS. Verify every channel before using electrodes.

## 3. Input-shorted noise test

`ads1299_configure_input_short_test()` selects CHnSET MUX `001` for all channels. TI documents this as shorting both channel inputs to the internal common-mode level for inherent-device/system noise measurements.

Record at minimum: board revision, gain, sample rate, VREF, clock source, supply topology, duration, and RMS/peak-to-peak noise calculation bandwidth.

## 4. Human-signal smoke test

Only after electrical and isolated/battery-powered safety review. Document electrode type, montage, reference, bias electrode, gain, sample rate and all digital filtering. Do not market this SDK as medical-device software.

Primary source: https://www.ti.com/lit/ds/symlink/ads1299.pdf
