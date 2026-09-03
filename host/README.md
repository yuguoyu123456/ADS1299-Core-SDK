# Host tools

The host layer is intentionally independent of MCU vendor. Every acquisition target should emit the common packet described in `PROTOCOL.md`.

Current Python pieces:

- `packet.py`: CRC16 + packet encode/decode
- `serial_reader.py`: stream resynchronization, sequence-gap warning

Planned:

- live 8-channel plot
- raw binary and CSV recorder
- metadata sidecar (sample rate, gain, VREF, channel names)
- MATLAB reader
- R reader
- MNE Raw conversion
- BIDS/EEG-BIDS export helper
