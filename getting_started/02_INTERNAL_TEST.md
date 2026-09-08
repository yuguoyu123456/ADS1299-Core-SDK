# 02 - Internal Test Signal

Run this immediately after the single-device ID test. Keep external electrodes disconnected.

## Goal

Prove that configuration, DRDY timing, SPI frame capture, 24-bit sign handling and PC transport all work before debugging real biopotential signals.

## Configure the SDK profile

Use the ready profile in `firmware/core_driver/ads1299/ads1299_profiles.h`:

- `ads1299_profile_internal_test(...)`

Apply the profile with the normal configuration path, then start continuous acquisition with the provided streaming helpers.

## Expected raw frame size

For ADS1299-8 in continuous mode the SDK expects exactly:

- 3 status bytes
- 8 channels × 3 bytes
- total = 27 bytes per DRDY

Do not read a variable-length frame in the DRDY path.

## Acquisition architecture

Recommended flow:

1. DRDY interrupt/event only marks that a sample is ready.
2. A high-priority acquisition context performs the exact 27-byte SPI read.
3. Store the frame in a bounded buffer/ring.
4. Lower-priority code converts/transports it.

Do not print, allocate memory, wait on a network socket or perform BLE/Wi-Fi work inside the DRDY ISR.

## What to inspect

Check both the raw and decoded representation:

- the frame rate should match the configured sample rate;
- status bytes should be structurally stable for the chosen mode;
- all enabled channels should show the internally generated test waveform;
- 24-bit negative values must sign-extend correctly;
- packet sequence numbers should increase monotonically on the host;
- CRC validation of the 49-byte SDK packet should pass.

The exact internal-test amplitude/frequency depends on the ADS1299 register configuration. Verify the configured setting against the device datasheet rather than assuming one universal waveform magnitude.

## Logic-analyzer check

Capture at least:

- DRDY
- CS
- SCLK
- MISO

For each DRDY used by the acquisition path, verify that one complete 27-byte frame is clocked without overlap into the next sample period.

## Failure patterns

### Flat zero/all-ones channels

Likely causes include MISO routing, wrong MUX/profile application, device still in the wrong command mode, or SPI timing problems.

### Correct-looking waveform on only some channels

Check channel register writes and the exact ADS1299 variant/channel configuration.

### Waveform shape changes when logging/networking starts

The acquisition path is being blocked or delayed. Move transport/logging out of the DRDY-critical context and use the bounded ring/queue helpers.

### Periodic lost frames

Measure acquisition latency, buffer high-water mark and transport back-pressure. Do not hide missing samples by duplicating old data.

## Pass criteria

Proceed only when the internal test runs continuously without unexplained frame loss and the PC receives valid, CRC-clean packets with monotonically increasing sequence numbers.
