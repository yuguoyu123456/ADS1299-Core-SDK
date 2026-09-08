# macOS and Linux Integration

This guide covers Python-based capture plus the path toward native/libusb integrations.

## 1. Python

Verify Python 3:

```bash
python3 --version
```

For serial/USB CDC/UART:

```bash
python3 -m pip install pyserial
```

## 2. Serial device names

Typical examples:

- Linux USB CDC: `/dev/ttyACM0`
- Linux USB-UART: `/dev/ttyUSB0`
- macOS USB serial: `/dev/cu.usbmodem*` or `/dev/cu.usbserial*`

Use the actual device enumerated by the OS.

Raw canonical stream example:

```bash
python3 ads1299_desktop_capture.py serial /dev/ttyACM0 --baud 921600 --raw session.bin --csv session.csv
```

Repository UART COBS example:

```bash
python3 ads1299_uart_cobs_capture.py /dev/ttyUSB0 --baud 921600 --raw session.bin --csv session.csv
```

## 3. Linux permissions

If the device cannot be opened, fix device permissions/udev configuration through the normal system administration process. Do not run a production acquisition application permanently as root simply to bypass permissions.

For a released USB device, document the VID/PID and provide an appropriate udev rule example once those IDs are actually assigned.

## 4. macOS permissions/device access

Document the actual USB/serial mode used by the supported hardware and verify it on current macOS versions. Avoid claiming a specific driver requirement unless tested with the chosen USB implementation.

## 5. Linux native path

For high-rate acquisition, Linux can use:

- USB bulk/libusb;
- TCP/UDP/Ethernet;
- local SPI/GPIO paths under `firmware/linux_compute/` when the ADS1299 is directly attached to an SBC.

Keep canonical packet encoding at the boundary between acquisition and higher-level PC analysis tools so the same parser/recorder can be reused.

## 6. macOS native path

A native macOS application should keep platform-specific USB/device-discovery code separate from the packet parser and recording model.

Protocol tests should run without hardware so packet format changes cannot silently break the macOS application.

## 7. Long-duration validation

On both platforms record:

- validated packet count;
- CRC/parser failures;
- sequence gaps;
- reconnects;
- disk-write errors;
- process CPU/memory behavior;
- actual sample/packet rate.

Repeat with the exact intended 64-channel transport load.

## 8. File-system durability

For research recordings:

- write to a local reliable filesystem;
- periodically flush through a controlled writer thread/process if required;
- do not block the device-reading thread on expensive CSV formatting;
- preserve the binary canonical data first;
- record metadata and final integrity statistics alongside the session.
