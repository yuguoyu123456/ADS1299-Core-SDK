# MIMXRT1166 + ADS1299 Ready-to-Run Guide

This file extends the existing MIMXRT1166 support without removing or replacing any existing project files.

## Recommended acquisition path

1. Use the existing NXP board/port layer in this directory.
2. Add the shared helper sources:
   - `../_shared/nxp_ads1299_ready_app.c`
   - `../_shared/nxp_ads1299_ready_app.h`
3. Add the common ADS1299 core driver and `ads1299_profiles.c` to the MCUXpresso project.
4. Configure LPSPI/SPI as master, CPOL=0, CPHA=1 (SPI Mode 1), MSB first.
5. Start bring-up at a conservative SPI clock around 2–4 MHz, then increase only after stable validation.
6. Configure CS, RESET, PWDN and START as outputs and DRDY as a falling-edge interrupt input.

## Runtime architecture

Keep the DRDY ISR short: only clear the interrupt and signal the acquisition context. Perform the exact 27-byte ADS1299-8 frame transfer outside the GPIO ISR. Store each frame in a ring buffer or queue, then let USB/Ethernet/UART/storage code consume frames independently.

For RT1166 multicore designs, keep one core or one high-priority task responsible for deterministic acquisition and isolate networking/UI/application workloads from the sampling path.

## Bring-up sequence

1. Reset ADS1299 and verify the ID register.
2. Apply `NXP_ADS1299_PROFILE_INTERNAL_TEST` and confirm a stable internal test waveform.
3. Apply `NXP_ADS1299_PROFILE_INPUT_SHORT` and confirm low-noise shorted-input behavior.
4. Apply `NXP_ADS1299_PROFILE_EEG_250` for first real-electrode validation.
5. Only after stable single-device capture, enable lead-off, BIAS and higher sample-rate profiles.

## DMA and cache notes

If LPSPI DMA is used, place DMA buffers in memory accessible by the selected DMA engine and handle cache clean/invalidate operations correctly. Do not let cache coherency bugs masquerade as ADS1299 SPI faults.

## 64-channel expansion

For 8 x ADS1299, use multiple SPI/DMA lanes or an FPGA capture front-end when tight inter-device skew matters. The MIMXRT1166 is well suited to control, packetization and high-speed transport after deterministic capture is established.

See `../DIRECT_USE.md` for the common NXP integration flow.