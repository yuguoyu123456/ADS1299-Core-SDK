# Desktop Integration Validation

Use this checklist before marking a desktop platform/transport combination as supported.

## A. Software parser

- [ ] `python test_desktop_capture.py` passes.
- [ ] Valid canonical packet decodes correctly.
- [ ] Corrupted CRC is rejected.
- [ ] Arbitrary byte chunk boundaries are handled.
- [ ] Parser recovers after junk/corruption.
- [ ] Negative signed channel values decode correctly.
- [ ] Sequence gap is detected and counted.
- [ ] UART COBS round trip passes.

## B. Device discovery/open

For each claimed OS:

- [ ] actual device can be discovered/opened;
- [ ] device identity/port is documented;
- [ ] permissions/driver requirements are documented;
- [ ] reconnect behavior is tested;
- [ ] two identical devices can be distinguished when that use case is supported.

## C. 8-channel capture

- [ ] one ADS1299 streams at 250 SPS;
- [ ] internal test is recorded;
- [ ] input short is recorded;
- [ ] raw canonical binary is saved;
- [ ] CSV decode matches raw packet fields;
- [ ] CRC errors are zero or fully explained;
- [ ] sequence gaps are zero or fully explained;
- [ ] timestamp cadence is plausible/consistent.

## D. Transport-specific validation

### Raw USB/TCP canonical stream

- [ ] one host read may contain partial/multiple packets without failure;
- [ ] disconnect does not produce silently corrupted samples;
- [ ] restart/reconnect creates an explicit session boundary.

### UART COBS

- [ ] zero-delimited COBS frames decode;
- [ ] deliberately corrupted COBS frame is rejected;
- [ ] following frame is recovered;
- [ ] selected baud rate has measured throughput margin.

### Ethernet/Wi-Fi block transport

- [ ] transport block is decoded before canonical packet parsing;
- [ ] every contained canonical packet CRC is validated;
- [ ] network reordering/loss policy is documented when UDP is used.

## E. 64-channel capture

At the intended sample rate:

- [ ] eight device streams/packets are mapped to global channels 1-64;
- [ ] one sample group contains exactly the intended eight device packets;
- [ ] host writer keeps up with sustained input;
- [ ] queue/buffer high-water behavior is measured;
- [ ] raw binary output remains lossless;
- [ ] packet loss/reordering is visible in final statistics;
- [ ] long-duration capture completes without unexplained disconnect/reset.

## F. File integrity

- [ ] raw file size is consistent with validated packet count × 49 bytes when storing only canonical packets;
- [ ] metadata identifies firmware commit, hardware revision, sample rate, gain, Vref/reference/clock configuration and channel map;
- [ ] CSV/derived files can be regenerated from the preserved raw data;
- [ ] final integrity statistics are stored with the recording.

## G. Platform release record

For Windows/macOS/Linux/ChromeOS configurations actually tested, record:

- OS/version;
- CPU architecture;
- Python/application version;
- transport/device mode;
- driver/permission setup;
- firmware commit;
- ADS1299 device count;
- sample rate;
- test duration;
- packets received;
- CRC/parser errors;
- sequence gaps;
- reconnects;
- known limitations.

Documentation or a passing parser unit test alone must not be presented as proof that a physical OS/device combination has been hardware-tested.
