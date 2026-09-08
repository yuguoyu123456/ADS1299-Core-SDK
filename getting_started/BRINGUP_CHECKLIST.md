# ADS1299 Bring-up Checklist

Use this as the acceptance checklist for a new board/MCU port. Do not skip ahead when a gate fails.

## A. Board and power

- [ ] Board revision recorded.
- [ ] ADS1299 device variant recorded.
- [ ] Analog and digital supply rails measured on hardware.
- [ ] Reference/clock source identified.
- [ ] RESET/PWDN/START wiring identified.
- [ ] SPI SCLK/MOSI/MISO/CS wiring identified.
- [ ] DRDY wiring identified.
- [ ] No guessed GPIO numbers copied from another target.
- [ ] Electrodes/person remain disconnected during initial digital bring-up.

## B. SPI and ID

- [ ] SPI configured Mode 1: CPOL=0, CPHA=1, MSB first.
- [ ] Conservative SPI clock used for first bring-up.
- [ ] RESET produces a repeatable known state.
- [ ] `SDATAC` used before register access when required.
- [ ] ADS1299 ID register reads successfully.
- [ ] ID is repeatable over many reads.
- [ ] Logic-analyzer capture confirms expected CS/SCLK/MOSI/MISO behavior.

## C. Internal test

- [ ] Internal-test profile applied successfully.
- [ ] DRDY toggles at the configured sample cadence.
- [ ] Exactly 27 bytes are read for each ADS1299-8 continuous frame.
- [ ] 3 status bytes are preserved.
- [ ] All 8 channel values are decoded in the correct order.
- [ ] 24-bit sign extension is correct.
- [ ] Internal waveform is visible on every enabled channel.
- [ ] No unexplained frame loss under normal logging/transport load.

## D. Input-short / noise

- [ ] Input-short profile applied.
- [ ] Raw unfiltered data recorded.
- [ ] Sample rate, PGA gain, reference and clock settings recorded.
- [ ] Mean/noise/peak-to-peak inspected for all channels.
- [ ] Channel-to-channel outliers investigated.
- [ ] Periodic interference investigated.
- [ ] Noise result is repeatable under the same setup.

## E. Canonical packet and PC validation

- [ ] One ADS1299 sample is encoded into the 49-byte canonical packet.
- [ ] Sync bytes `0xA5 0x5A` are detected.
- [ ] CRC16 validates on the host.
- [ ] Sequence number increments monotonically.
- [ ] Device timestamp is decoded consistently.
- [ ] Status bytes survive transport unchanged.
- [ ] Eight channels survive transport unchanged.
- [ ] Corrupted-packet test is rejected by CRC.
- [ ] Sequence-gap counters are visible to the recorder/operator.

## F. Transport

### USB

- [ ] Arbitrary host read boundaries are handled by the stream parser.
- [ ] Multiple canonical packets can be batched without changing their contents.
- [ ] Back-pressure does not block DRDY-critical acquisition.

### UART

- [ ] COBS + zero delimiter framing works.
- [ ] Receiver resynchronizes after a deliberately corrupted frame.
- [ ] Actual baud-rate budget has margin for selected sample rate/device count.

### BLE

- [ ] Actual application notification payload size is known.
- [ ] 49-byte packet fragments and reassembles correctly.
- [ ] Missing/out-of-order fragment handling is tested.
- [ ] Radio stack never blocks acquisition ISR.

### Ethernet / Wi-Fi

- [ ] UDP/TCP block framing validates all contained canonical packets.
- [ ] Disconnect/reconnect behavior is tested.
- [ ] Queue overflow policy and counters are defined.
- [ ] Network jitter does not corrupt acquisition cadence.

### LSL

- [ ] LSL runs as a host-side bridge after canonical packet validation.
- [ ] Channel labels/order are fixed.
- [ ] Device timestamp/sequence remain available for audit.

## G. First EEG

- [ ] Complete system electrical-safety/isolation design has been reviewed before human connection.
- [ ] Internal-test mode is definitely disabled before live EEG.
- [ ] 250 SPS EEG profile is applied.
- [ ] Reference/bias/electrode configuration matches the real hardware plan.
- [ ] Raw unfiltered data is saved.
- [ ] Live display does not replace raw recording.
- [ ] Channel labels/electrode locations are recorded.
- [ ] Signal responds plausibly to controlled physiological/artifact changes.

## H. Scale to 16 / 32 / 64 channels

For each stage 2, 4 and 8 ADS1299 devices:

- [ ] Every device ID is stable.
- [ ] Every channel passes internal test.
- [ ] Every channel passes input-short/noise capture.
- [ ] Device index and channel order are documented.
- [ ] Common clock/START/RESET strategy is documented.
- [ ] DRDY relationship is measured.
- [ ] Inter-device skew is measured when relevant.
- [ ] Multi-device sample grouping is deterministic.
- [ ] Transport bandwidth is calculated from actual encoded bytes.
- [ ] Queue high-water mark and overflow counters are recorded.
- [ ] Long-duration target-load capture shows no unexplained loss.

## I. Release evidence

A target is ready to be called "validated on hardware" only when evidence exists. Record:

- [ ] board revision;
- [ ] MCU/FPGA/host target;
- [ ] firmware commit SHA;
- [ ] build/toolchain version;
- [ ] sample rate and gain;
- [ ] number of ADS1299 devices/channels;
- [ ] transport;
- [ ] test duration;
- [ ] packet/frame loss counters;
- [ ] relevant logic-analyzer/oscilloscope captures;
- [ ] measured noise results;
- [ ] known limitations.

Software documentation and unit tests alone must not be labeled as completed electrical, EMC, medical-safety or production validation.
