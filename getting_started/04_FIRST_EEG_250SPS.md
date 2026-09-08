# 04 - First EEG at 250 SPS

Only perform this step after ID, internal-test and input-short validation have passed.

## Safety gate

Do not connect a person to an unreviewed bench-powered system. The complete system must have an appropriate electrical-safety/isolation design for human-connected biopotential acquisition. USB-grounded prototypes, exposed conductors and improvised power arrangements require particular caution.

## Goal

Acquire one ADS1299 device (8 channels) at 250 SPS with a simple, controlled electrode setup and verify that the observed signal changes plausibly with the test condition without losing raw data.

## Recommended SDK profile

Use the ready EEG configuration:

- `ads1299_profile_eeg_250sps(...)`
- apply through the profile/configuration helper;
- start continuous streaming through the provided start helper.

Keep the exact PGA gain, reference and channel MUX settings recorded with the capture.

## Start simple

For the first live recording:

- use only the channels needed to prove the signal path;
- keep cable routing short and controlled;
- keep wireless/network traffic optional until the wired/raw path is stable;
- use a known reference/bias strategy from the actual hardware design rather than inventing one in firmware;
- record raw unfiltered data in addition to any live display.

## Acquisition flow

1. DRDY event occurs.
2. Read exactly one 27-byte ADS1299-8 raw frame.
3. Parse status + eight signed 24-bit samples.
4. Add sequence/timestamp metadata.
5. Encode the repository's canonical 49-byte packet.
6. Queue it to USB/UART/Ethernet/Wi-Fi/BLE transport outside the DRDY ISR.
7. On the host, verify sequence and CRC before plotting.

## First physiological sanity checks

Use simple observations only as a sanity check, not as a diagnostic conclusion. For example, a controlled eyes-open/eyes-closed recording from an appropriate posterior montage may show a change in alpha-band activity in some users. Motion, blinking and jaw activity should also create recognizable artifacts when the relevant electrodes are affected.

If the waveform looks 'too perfect', verify that the device has actually left internal-test mode.

## Do not hide problems with filters

Keep an unfiltered raw file. Apply display filters only after confirming:

- no packet gaps;
- correct channel order;
- correct sample rate;
- correct signed conversion;
- correct Vref/PGA scaling metadata.

## Minimum metadata for every recording

Store:

- device/board revision;
- firmware commit;
- ADS1299 count and channel count;
- sample rate;
- PGA gain;
- reference/clock configuration;
- channel/electrode labels;
- transport;
- start time and device timestamp basis;
- any digital filters used for display or export.

## Pass criteria

This step passes when a continuous raw recording can be captured, decoded and replayed with correct channel order, stable sample cadence and no unexplained CRC/sequence failures, and the live signal responds plausibly to controlled physiological/artifact changes.
