# ESP32-S3 ADS1299 capability

| Item | Verified planning fact |
|---|---|
| CPU | Dual-core Xtensa LX7, up to 240 MHz |
| RAM | 512 KiB on-chip SRAM; the pinned N8 PlatformIO board exposes a 320 KiB application budget |
| Flash | 8 MiB in the `esp32-s3-devkitc-1` PlatformIO board definition |
| SPI | SPI2/SPI3 are general-purpose masters; reference uses SPI2, Mode 1 at 4 MHz |
| DMA | ESP-IDF SPI master supports DMA; the reference uses polling transactions with internal buffers |
| USB | USB 2.0 OTG plus USB Serial/JTAG; GPIO19/20 are kept free |
| Radio | 2.4 GHz Wi-Fi and Bluetooth LE 5 |

## ADS1299 planning boundary

One ADS1299 frame is acquired through the shared Core and hardware-only Port.
Radio, USB and host packetization belong in `firmware/transport/`; they must not
run inside a DRDY ISR or hold the SPI bus. Multi-device use is feasible only
after measuring DRDY service latency, buffer headroom and loss under radio load.
No throughput or hardware count is claimed by this compile-only reference.

Sources and exact build evidence are recorded in `sources.md` and `build.md`.
