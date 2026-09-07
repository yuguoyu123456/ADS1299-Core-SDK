# ADS1299 transport layer

This directory moves complete ADS1299 sample packets from an acquisition task
to a host link. It does not access ADS1299 registers, SPI, CS, DRDY, RESET or
START. Those responsibilities remain in the Core Driver and MCU/FPGA Port.

`ads1299_transport.[ch]` provides a small callback contract for bounded writes,
partial-write handling and optional flush. `ads1299_transport_send_sample()`
encodes the repository's versioned 49-byte packet from
`firmware/common/data_packet/` before submission.

## Data path

```text
ADS1299 -> Core Driver -> acquisition queue -> Transport -> PC/mobile/edge host
```

Never call a blocking transport from the DRDY interrupt. Copy the parsed frame
to a bounded queue and transmit it from a lower-priority task. Queue overflow,
sequence gaps and reconnect behaviour must be visible to the application.

## Backends

| Backend | Directory | Intended boundary |
|---|---|---|
| UART | `uart/` | byte stream supplied by the vendor SDK |
| USB | `usb/` | CDC/bulk endpoint supplied by the vendor SDK |
| BLE | `ble/` | notification transport with negotiated payload size |
| Wi-Fi | `wifi/` | TCP/UDP socket supplied by the network stack |
| Ethernet | `ethernet/` | TCP/UDP socket supplied by the network stack |
| LSL | `lsl/` | host-side bridge; not an MCU protocol implementation |

The repository does not bundle vendor middleware or claim these backends are
bench-tested. Each platform must retain its own truthful validation status.
