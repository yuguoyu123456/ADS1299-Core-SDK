# ADS1299 MCU Reference App Validation

This document is additive and does not replace the existing reference application.

## 1. Port-layer sanity

- SPI must be ADS1299 mode 1: CPOL=0, CPHA=1.
- Start with a conservative 2-4 MHz SCLK.
- Verify CS idles high and remains asserted for each complete command/frame transaction.
- Verify RESET, PWDN and START levels with a logic analyzer.
- Verify DRDY falling edges reach the acquisition context without long ISR work.

## 2. Device identification

Read the ID register before configuration. Confirm the decoded ADS1299-family variant and expected 4/6/8-channel count. Treat an unexpected ID as a bring-up failure rather than continuing with normal EEG settings.

## 3. Internal digital/analog path test

Apply the internal test-source profile first. Capture at least several seconds and verify:

- status header begins with the expected `0xC` nibble;
- every DRDY produces one complete frame;
- no SPI/read errors or sequence gaps occur;
- the calibration waveform is repeatable on all enabled channels.

Then apply the input-short profile and verify stable low-noise ADC/PGA output before electrodes are connected.

## 4. Normal EEG profile

Start at 250 SPS. For ADS1299-8, each RDATAC transaction must contain exactly 27 bytes: 3 status + 8 x 3 channel bytes. Only after this path is stable should 500 SPS or 1000 SPS profiles be enabled.

## 5. Buffer validation

`ads1299_frame_ring.[ch]` provides a fixed-capacity reference queue with sequence, timestamp and overflow counters. It is intentionally allocation-free.

The reference ring does not implement cross-context locking. If producer and consumer execute in different interrupt/task/core contexts, protect index/count updates using the target's appropriate critical section, atomic primitive, lock-free SPSC implementation or RTOS queue. Do not assume `volatile` alone provides synchronization.

Stress the transport/consumer until the queue approaches capacity. A queue overflow must increment a visible counter; it must never be silently ignored.

## 6. Lead-off and BIAS

After clean normal acquisition is proven, enable BIAS/reference and lead-off features according to the board analog design. Re-check noise, common-mode behavior and channel status.

## 7. Multi-device / 64-channel validation

For 8 x ADS1299:

1. verify each device ID independently;
2. run internal test on every device;
3. verify common START/clock strategy and DRDY behavior;
4. measure inter-device frame timing/skew;
5. stress the full transport bandwidth;
6. record sequence gaps and buffer overruns per device.

For tight deterministic timing, use the common FPGA capture layer in `firmware/fpga/common` and let the MCU perform control, buffering and transport.

## Acceptance checklist

A port is ready for hardware-specific qualification when it can pass:

`ID -> internal test -> input short -> 250 SPS EEG -> higher rate if required -> lead-off/BIAS -> buffer/transport stress -> multi-device synchronization`

Passing this software checklist is not a claim of board-level electrical safety, EMC, medical certification or production hardware validation.
