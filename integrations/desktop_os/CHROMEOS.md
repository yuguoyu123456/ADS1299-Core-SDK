# ChromeOS / Linux-Container Workflow

ChromeOS support depends on the device-access path available on the specific Chromebook and ChromeOS configuration.

## Recommended approach

When Linux development environment/container access is available, treat the workflow as a Linux host integration:

1. make the USB/serial device available to the Linux environment through the supported ChromeOS UI/mechanism;
2. verify the device appears inside Linux;
3. install Python/pyserial if serial access is used;
4. run the same canonical packet recorder/validator used on Linux.

Example once the device is visible as a Linux serial node:

```bash
python3 ads1299_desktop_capture.py serial /dev/ttyACM0 --baud 921600 --raw session.bin --csv session.csv
```

For repository UART COBS framing:

```bash
python3 ads1299_uart_cobs_capture.py /dev/ttyUSB0 --baud 921600 --raw session.bin --csv session.csv
```

## Important limitations

Do not claim universal ChromeOS USB access. Availability depends on:

- Chromebook model;
- ChromeOS version/policy;
- Linux-container support;
- whether the specific USB device can be shared into the container;
- administrator restrictions on managed devices.

## Network fallback

If direct USB access is not available, a practical architecture is:

`ADS1299 device -> Ethernet/Wi-Fi/TCP -> Chromebook Linux container`

The desktop TCP parser can then consume a raw canonical packet stream. If the network sender uses the repository Ethernet transport block format, decode that block format first.

## Validation

For a claimed supported ChromeOS setup, record:

- Chromebook model;
- ChromeOS version;
- device transport;
- Linux environment version;
- device node/network path;
- packet rate;
- CRC/parser failures;
- sequence gaps;
- reconnect behavior;
- tested firmware commit.

ChromeOS integration should be described as tested only for configurations that have actually been validated.
