# Integration with ESP-IDF

1. Install the official `ESP-IDF` environment; do not copy the vendor SDK into this repository.
2. Use the official **ESP8684-DevKitM-1 v1.1** reference path first when possible.
3. Edit hardware-dependent pins and the SPI clock only in `board/esp8684_devkitm1_ads1299.h`.
4. Build the standalone project in `examples/esp_idf_reference/` before adapting the port to another ESP32-C2 board.
5. Keep SPI as Mode 1, MSB first, software-controlled CS, and a conservative 4 MHz reference clock.
6. Keep transport work outside the DRDY/SPI path; the reference application uses a bounded queue and separate acquisition/transport tasks.
7. First verify power, ID read, internal test and input-short. Only then move to normal 250-SPS electrode-input streaming.

The `spi_transfer` callback is full duplex. A null TX pointer means clock zero
bytes while receiving; a null RX pointer means discard received bytes. The
ESP-IDF adapter implements that portable callback contract rather than pushing
ESP-IDF semantics into the shared ADS1299 core. CS is controlled by the separate
GPIO callback and is not toggled inside `spi_transfer`.

## SDK ownership boundary

Vendor startup, linker scripts, generated configuration and middleware remain
owned by ESP-IDF. This repository supplies the thin ESP32-C2 adapter, reference
board configuration, bounded acquisition example and tests. ADS1299 register
and command behavior remains in `firmware/core_driver/ads1299/`.

## Concrete reference path: ESP8684-DevKitM-1 v1.1

The official Espressif ESP8684-DevKitM-1 is the reference board for this folder.
ESP32-C2 currently maps to the ESP8684 series in Espressif's development-board
documentation.

Repository files used by the reference project:

- `board/esp8684_devkitm1_ads1299.h` — the one obvious board-dependent pin/clock file;
- `ads1299_port/ads1299_espidf_hal.[ch]` — ESP-IDF SPI/GPIO/delay binding;
- `ads1299_port/ads1299_spi.[ch]`, `ads1299_gpio.[ch]`, `ads1299_drdy.[ch]` — portable platform adapter;
- `examples/esp32c2_frame_queue.[ch]` — fixed 16-frame sustained-acquisition queue;
- `examples/esp_idf_reference/` — standalone runnable ESP-IDF project;
- `../../../core_driver/ads1299/` — shared ADS1299 core, frame/model/multi-device logic.

The default reference assignment is:

| ADS1299 signal | ESP8684-DevKitM-1 |
|---|---:|
| SCLK | GPIO6 |
| DIN / MOSI | GPIO7 |
| DOUT / MISO | GPIO2 |
| CS | GPIO10 |
| DRDY | GPIO3 |
| RESET | GPIO4 |
| PWDN | GPIO5 |
| START | GPIO18 |

The assignment deliberately avoids GPIO8/GPIO9 strapping use, UART0 GPIO19/20,
and GPIO0/GPIO1 used by RGB LED channels on board revision v1.1. Re-check the
exact board revision before wiring.

From an activated ESP-IDF environment:

```sh
cd firmware/mcu/02_Espressif/ESP32-C2/examples/esp_idf_reference
idf.py set-target esp32c2
idf.py build
idf.py -p <serial-port> flash monitor
```

The reference application executes this progressive flow automatically:

```text
hardware reset + SDATAC
        |
        v
ADS1299 family probe / channel-count detection
        |
        v
internal-test profile -> 8 diagnostic frames
        |
        v
STOP + SDATAC
        |
        v
input-short profile -> 8 diagnostic frames
        |
        v
STOP + SDATAC
        |
        v
250-SPS normal-input EEG configuration
        |
        v
DRDY falling-edge interrupt
        |
        v
higher-priority acquisition task
        |
        v
16-frame bounded queue
        |
        v
lower-priority transport task
```

The 250-SPS step uses shared typed APIs (`ads1299_set_data_rate`,
`ads1299_set_channel`, `ads1299_set_srb1`) rather than platform-local CONFIG1 or
CHnSET literals. The conservative reference selects gain 24, normal input,
SRB1 off and SRB2 off for every channel reported by the probed ADS1299-family
device. BIAS, lead-off, montage and electrode-specific routing remain explicit
application choices.

For sustained streaming, GPIO3 is configured for a falling-edge interrupt. The
ISR does no SPI, logging, packetization or wireless work; it only wakes the
higher-priority acquisition task through a FreeRTOS task notification. The
acquisition task then reads one complete ADS1299 frame and pushes it into the
bounded queue. The lower-priority transport task owns logging/UART/Wi-Fi/BLE or
canonical-packet work. Task-notification counts are consumed one at a time so a
brief scheduling delay does not intentionally collapse several DRDY events into
a single read.

Expected successful bring-up messages include:

```text
probe OK: ADS1299-family ID=0x.. channels=8
internal-test: capturing 8 frames
internal-test frame=0 status=...... ch1=...
input-short: capturing 8 frames
input-short frame=0 status=...... ch1=...
250-SPS EEG profile ready: channels=8 gain=24 normal-input SRB1=off SRB2=off
DRDY falling-edge notification enabled on GPIO3
250-SPS EEG streaming started: event-driven DRDY, bounded queue=16 frames
beginner flow complete: probe -> internal-test -> input-short -> EEG250 stream
EEG250 frame seq=0 t=...us status=...... ch1=...
```

A missing DRDY edge for one second is reported separately from an SPI/frame-read
failure. Queue-full events increment `dropped` and expose `high_watermark`; they
never silently overwrite unread EEG frames.

Host-only queue regression can be built independently:

```sh
cd firmware/mcu/02_Espressif/ESP32-C2/tests
make -f Makefile.host clean
make -f Makefile.host test
```

Validation status remains **TEMPLATE / PLANNED** until the current source state
is actually compiled with ESP-IDF and run on physical hardware. Source presence
does not imply BUILD-VERIFIED or BOARD-VERIFIED.
