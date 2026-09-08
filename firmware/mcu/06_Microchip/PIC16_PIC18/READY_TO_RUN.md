# PIC16 / PIC18 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing PIC16/PIC18 support remains unchanged.

## MPLAB / MCC integration

Reuse the current `ads1299_port/` and board abstraction. Bind the generic ADS1299 callbacks to MCC/Melody or register-level MSSP SPI, GPIO and interrupt resources, then add the shared Microchip ready-app helper and common ADS1299 core/profile sources.

## Peripheral setup

- MSSP/SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Select a conservative SPI clock compatible with the selected MCU/system clock; begin around 0.5–2 MHz where practical.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge external interrupt input.

## Resource-conscious acquisition

Keep the DRDY ISR minimal: set a flag or signal the foreground acquisition loop. Read exactly one 27-byte ADS1299-8 frame and place it in a small static buffer/ring buffer.

Do not perform floating-point conversion, printf or packet formatting in the sampling ISR.

## Validation order

1. Hardware reset ADS1299.
2. Read and verify device ID.
3. `MICROCHIP_ADS1299_PROFILE_INTERNAL_TEST`.
4. `MICROCHIP_ADS1299_PROFILE_INPUT_SHORT`.
5. `MICROCHIP_ADS1299_PROFILE_EEG_250`.
6. Confirm sustained frame alignment and serial-link headroom.
7. Add optional BIAS/lead-off after stable baseline capture.

## Throughput note

For small PIC devices, forward raw channel codes and perform voltage conversion/advanced DSP on the host. Calculate transport bandwidth before enabling higher sample rates and expose overflow counters rather than silently dropping frames.

## 64-channel note

PIC16/PIC18 devices are not the recommended primary acquisition engine for 8 x ADS1299. Use FPGA or a higher-throughput MCU/front-end for synchronized 64-channel capture.

See `../DIRECT_USE.md` for the common Microchip flow.