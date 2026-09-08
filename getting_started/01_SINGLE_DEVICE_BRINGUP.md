# 01 - Single ADS1299 Bring-up

The first target is one ADS1299-8 device. Do not start with eight devices at once.

## Goal

Prove the complete digital control path:

`MCU/FPGA -> RESET/CS/SCLK/MOSI -> ADS1299 -> MISO/DRDY -> host`

## Required signals

Map the real board schematic to the platform adapter. At minimum, identify:

- SPI SCLK
- SPI MOSI/DIN
- SPI MISO/DOUT
- CS
- DRDY
- RESET
- START when driven by the host
- CLK source/configuration
- PWDN when used by the board

Do not invent GPIO numbers. The SDK keeps board-specific pin choices outside the portable ADS1299 core.

## Step 1: idle levels

Before talking to the device:

- CS should remain inactive except during an SPI transaction.
- RESET must be released according to the actual hardware design.
- SPI must be Mode 1, MSB first.
- Do not allow unrelated RTOS tasks, radio stacks or logging code to toggle ADS1299 pins.

## Step 2: reset and command state

After hardware reset, place the device in a known command state before register access. If continuous data mode may already be active, send `SDATAC` before configuration/register reads.

The profile layer already exposes:

- `ads1299_sdatac(...)`
- EEG/test/shorted-input profile helpers in `firmware/core_driver/ads1299/ads1299_profiles.h`

## Step 3: read the device ID

Read the ADS1299 ID register using the core driver. Treat this as the first mandatory hardware gate.

A valid read should be:

- repeatable across many reads;
- unchanged when ordinary debug logging is enabled/disabled;
- not all `0x00`;
- not all `0xFF`;
- consistent with the populated ADS1299 device variant.

Do not hard-code a guessed ID value for every possible ADS1299-family part. Validate against the exact device fitted to the board and the TI datasheet used for that hardware revision.

## If ID read fails

Check in this order:

1. power rails and ground reference;
2. RESET/PWDN state;
3. CS polarity and timing;
4. SPI CPOL/CPHA;
5. MOSI/MISO routing;
6. SCLK frequency;
7. whether RDATAC must first be stopped with SDATAC;
8. clock source/configuration;
9. board-level solder/open/short faults.

A logic analyzer capture of CS/SCLK/MOSI/MISO is usually more useful than adding delays blindly.

## Step 4: configure one device only

Do not enable BIAS, lead-off, daisy-chain or multi-device synchronization during the first ID test. Keep the configuration minimal so that each failure has one likely cause.

## Pass criteria

This stage is complete when:

- repeated ID reads are stable;
- SPI transactions have the expected Mode-1 timing;
- RESET reliably returns the device to a known state;
- no SPI errors appear when the host performs repeated register reads;
- the next internal-test profile can be applied without changing board wiring.
