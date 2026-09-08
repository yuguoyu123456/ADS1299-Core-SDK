# USB CDC / serial interoperability

This directory defines how ADS1299-Core canonical packets should be carried over wired byte-stream transports without changing the repository-wide sample model.

## Two distinct framing modes

Do not treat all serial-looking links as identical.

### USB CDC / USB Bulk style raw stream

For USB transports that present an ordered byte stream to the host, canonical packets may be concatenated directly:

```text
[49-byte packet][49-byte packet][49-byte packet]...
```

The receiver must tolerate arbitrary host read boundaries. One USB read is not guaranteed to equal one ADS1299 packet.

Use sync `0xA5 0x5A`, fixed packet length and CRC16 validation to recover packets from the stream.

### UART serial framing

The repository UART transport wraps each canonical 49-byte packet in COBS and terminates it with `0x00`:

```text
COBS(49-byte canonical packet) 00
```

The receiver must COBS-decode first, then validate the recovered canonical packet.

## Canonical packet remains unchanged

After outer transport deframing, the payload is always the same 49-byte ADS1299-Core packet containing:

- protocol version;
- flags;
- uint32 sequence;
- uint32 device timestamp;
- three ADS1299 status bytes;
- eight signed channel samples;
- CRC16-CCITT-FALSE.

## Related repository implementations

- firmware USB stream helper: `firmware/transport/usb/`
- firmware UART COBS helper: `firmware/transport/uart/`
- desktop recorder/parser: `integrations/desktop_os/ads1299_desktop_capture.py`
- UART desktop recorder: `integrations/desktop_os/ads1299_uart_capture.py`

## 64-channel note

Eight ADS1299 devices produce eight canonical packets per synchronized sample epoch. A wired host can batch these for efficiency, but batching must not destroy device identity or channel mapping.

Use explicit device/source metadata at the integration layer and keep deterministic global channel mapping.

## Validation status

This directory specifies repository interoperability behavior. Actual maximum throughput, USB endpoint configuration, UART baud rate and OS/device compatibility must be measured on the intended hardware and host system rather than inferred from protocol format alone.
