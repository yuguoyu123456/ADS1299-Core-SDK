# Other MCU/DSP/RISC-V Targets + ADS1299 Direct-Use Guide

This guide is additive and preserves every existing target-specific `ads1299_port` file.

Use the target's SPI peripheral as an 8-bit, MSB-first ADS1299 mode-1 master (CPOL=0, CPHA=1). Start around 2-4 MHz during bring-up. Provide board-specific CS, RESET, PWDN, START and falling-edge DRDY handling through the existing callback port.

Recommended runtime flow:

1. Initialize clocks, GPIO and SPI.
2. Build the existing `ads1299_port_t` callback object.
3. Initialize the ADS1299 core and verify the device ID.
4. Run the internal-test profile.
5. Run the input-short profile.
6. Apply the 250 SPS EEG profile.
7. Start RDATAC streaming.
8. On every DRDY event, service exactly one complete ADS1299-8 frame (27 bytes).
9. Push the completed frame into a bounded ring/queue for downstream processing or transport.

Keep the DRDY ISR minimal. First prove a blocking transfer with the ADS1299 internal test source. Add DMA/non-blocking transfers only after the blocking path is correct. On DSP, multicore or application-class targets, isolate acquisition from heavy DSP/AI/network work. On low-resource targets, keep fixed-size buffers and prefer raw 24-bit data.

For 8 x ADS1299 / 64-channel systems, use multiple deterministic capture lanes when the target exposes them. For tight inter-device skew, use `firmware/fpga/common` as the preferred capture front end and use the MCU/DSP for control, buffering, processing and transport.

Validation order:

`ID -> internal test -> input short -> 250 SPS EEG -> higher rate where practical -> lead-off/BIAS -> transport/processing stress -> multi-device synchronization`
