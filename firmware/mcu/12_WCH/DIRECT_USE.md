# WCH CH32 Direct ADS1299 Use

This file adds a reusable ADS1299 integration path for CH32 targets. Existing per-device ports and examples remain unchanged.

## Integration rules

Reuse each model's existing `ads1299_port/` and bind it to the WCH peripheral library or the project's SPI/GPIO/EXTI implementation.

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz; for CH32V003 begin lower if needed by the system clock.
- CS/RESET/PWDN/START outputs; DRDY falling-edge interrupt input.
- ADS1299-8 continuous frame = 27 bytes.

Recommended path:

`DRDY -> short ISR -> acquisition context -> exact 27-byte frame -> fixed ring buffer -> USB/UART/Ethernet/application`

Add `12_WCH/_shared/wch_ads1299_ready_app.c/.h` plus the common ADS1299 core/profile sources.

Profiles: EEG 250/500/1000 SPS, internal test and input short.

## Validation

1. Hardware reset and ID verification.
2. Internal test.
3. Input short.
4. EEG 250 SPS.
5. Long continuous capture with frame alignment/read-error/overflow monitoring.
6. Stress the final transport.
7. Add BIAS/lead-off and faster rates only after stability.

## DMA/resource guidance

Small CH32 devices should use static buffers, raw 24-bit samples and minimal ISR work. Higher-performance CH32V3xx devices can add DMA after a synchronous reference path is proven.

## 64-channel architecture

For eight ADS1299 devices, use FPGA or multiple independent SPI/DMA paths when tight cross-device timing matters. CH32 can then handle control, buffering and transport according to its resource class.

Pin assignments remain board-specific.