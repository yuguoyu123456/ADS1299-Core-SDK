# 00 - Prerequisites and Safety Gate

Use this checklist before writing firmware or connecting electrodes.

## Minimum hardware

- One ADS1299 core/module or a board that exposes the ADS1299 digital interface.
- A supported MCU/FPGA/Linux host from `firmware/`.
- Stable analog and digital supplies appropriate for the actual board design.
- SPI signals: SCLK, DIN/MOSI, DOUT/MISO, CS.
- Control/status signals used by the selected design: DRDY, RESET, START, CLK/PWDN as applicable.
- A logic analyzer or oscilloscope is strongly recommended for bring-up.

Do not copy fixed GPIO numbers from another board. Select pins from the real board schematic and the selected MCU SDK.

## Firmware pieces used by this guide

- ADS1299 driver: `firmware/core_driver/ads1299/`
- ADS1299 ready profiles: `ads1299_profiles.h/.c`
- Common 49-byte packet format: `firmware/common/data_packet/`
- MCU examples: `firmware/mcu/`
- USB/UART/BLE/Wi-Fi/Ethernet/LSL helpers: `firmware/transport/`

## SPI baseline

The ADS1299 interface used by this SDK is SPI Mode 1:

- CPOL = 0
- CPHA = 1
- MSB first

Start with a conservative SPI clock during bring-up. Increase speed only after ID, internal-test and continuous-frame validation are stable.

## Mandatory bring-up order

1. Verify supplies, reference/clock configuration and RESET behavior on the actual board.
2. Keep electrodes disconnected.
3. Read and validate the ADS1299 device ID.
4. Run the internal test signal.
5. Run input-short/noise validation.
6. Start one-device 250 SPS continuous acquisition.
7. Validate the 27-byte ADS1299-8 raw frame and 49-byte SDK packet on the PC.
8. Only then proceed to electrode/EEG work on an electrically safe, properly isolated system.
9. Scale from 1 device to multiple ADS1299 devices only after the single-device path is repeatable.

## Electrical safety boundary

This repository is firmware/software guidance, not a declaration that a particular PCB is safe for human connection. Before connecting a person, the complete powered system must be reviewed for patient/user isolation, leakage-current risk, grounding, protection, enclosure, connector exposure and applicable regulatory requirements.

For first bring-up, use internal-test and shorted-input modes. They exercise the digital acquisition path without requiring electrodes.

## Definition of "pass"

A step is complete only when its observable result matches the checklist in that step. A successful compile alone is not hardware validation.
