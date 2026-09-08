# USB CDC / serial validation checklist

## Raw USB/CDC canonical stream

- [ ] receiver handles arbitrary read chunk sizes;
- [ ] sync recovery works after injected garbage bytes;
- [ ] wrong CRC packets are rejected;
- [ ] sequence gaps are counted;
- [ ] reconnect behavior is defined;
- [ ] long-duration capture does not show unbounded buffering;
- [ ] tested sample rate and channel count are recorded;
- [ ] tested host OS and device path are recorded.

## UART / COBS

- [ ] each encoded frame ends in `0x00`;
- [ ] COBS decode returns exactly 49 bytes;
- [ ] canonical sync/version/CRC is validated after COBS decode;
- [ ] a dropped/corrupt frame does not prevent recovery at the next delimiter;
- [ ] configured baud rate includes adequate margin for the intended sample rate;
- [ ] UART framing configuration matches on both ends;
- [ ] long cables/noisy environments are tested if relevant to deployment.

## Multi-device / 64-channel

- [ ] eight device/source identities remain stable;
- [ ] one synchronized epoch maps to exactly 64 channels;
- [ ] host batching preserves device identity and sequence;
- [ ] sustained 392 canonical bytes/epoch is supported at the intended sample rate;
- [ ] missing/reordered packets are detected instead of silently shifting channel mapping.

## Measurement record

For every claimed working configuration, record:

- hardware board/revision;
- MCU/FPGA firmware revision;
- USB/UART mode;
- baud rate or USB class/endpoint configuration;
- host OS and version;
- sample rate;
- number of ADS1299 devices;
- duration of soak test;
- observed sequence gaps/CRC failures;
- average and peak host CPU/memory where relevant.

Passing this software/transport checklist does not validate analog EEG quality, electrical isolation, EMC or medical-device safety.
