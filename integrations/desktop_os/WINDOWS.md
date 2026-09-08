# Windows Integration

Use this guide for Windows 10/11 desktop capture and validation.

## 1. Python setup

Install a current Python 3 release and verify:

```powershell
python --version
```

For serial/USB CDC/UART access:

```powershell
python -m pip install pyserial
```

## 2. Find the COM port

Use Device Manager or PowerShell to identify the COM port assigned to the device. Do not hard-code a COM number across machines.

For a raw canonical USB CDC stream:

```powershell
python ads1299_desktop_capture.py serial COM5 --baud 921600 --raw session.bin --csv session.csv
```

For repository UART COBS framing:

```powershell
python ads1299_uart_cobs_capture.py COM5 --baud 921600 --raw session.bin --csv session.csv
```

Use the actual firmware baud rate.

## 3. WinUSB / USB bulk

For high-rate 32/64-channel systems, WinUSB/bulk-style USB is generally preferable to pretending every transport is a COM port. The firmware USB transport already treats host reads as arbitrary byte chunks and preserves canonical packet boundaries through sync + CRC recovery.

A future native Windows application should reuse the same canonical parser logic rather than inventing a separate packet format.

## 4. Recording location

Use a local disk path with enough free space for long-duration raw capture. Record binary canonical packets first; create CSV/analysis files as derived outputs.

Avoid writing directly to slow network shares during acquisition unless the buffering/back-pressure behavior has been validated.

## 5. Driver/device behavior

Document for each supported board:

- USB VID/PID when applicable;
- CDC vs WinUSB/bulk mode;
- expected device/COM name;
- whether a custom INF/driver package is required;
- tested Windows version;
- tested firmware commit.

Do not claim plug-and-play support until it has been tested on a clean Windows machine.

## 6. Long-duration validation

Run a capture at the target sample rate/device count and record:

- parser/CRC errors;
- sequence gaps;
- device reconnects;
- host read failures;
- disk-write errors;
- effective packet rate;
- CPU/memory behavior of the desktop application.

For 64 channels, test with the exact intended transport and at least the full target throughput rather than one 8-channel stream.

## 7. Development recommendation

Keep the protocol parser cross-platform and place Windows-specific code only around:

- device discovery;
- WinUSB/serial opening;
- GUI packaging;
- installer/signing;
- Windows-specific logging/diagnostics.

This avoids protocol drift between Windows, macOS and Linux tools.
