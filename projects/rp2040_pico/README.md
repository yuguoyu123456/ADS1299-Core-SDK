# Raspberry Pi Pico / RP2040 — native Pico SDK reference project

Status: **Source-complete; CI build pending on this branch. Hardware validation pending.**

The project targets the official Raspberry Pi Pico SDK. CI is pinned to Pico SDK **2.3.0** so builds are reproducible instead of silently following `master`.

## Wiring

| ADS1299-Core | RP2040 Pico | Purpose |
|---|---|---|
| DOUT/MISO | GP16 | SPI0 RX |
| CS | GP17 | software CS |
| SCLK | GP18 | SPI0 SCK |
| DIN/MOSI | GP19 | SPI0 TX |
| DRDY | GP20 | falling-edge IRQ |
| RESET | GP21 | ADS1299 reset |
| START | GP22 | ADS1299 START |
| DGND | GND | digital ground |

Host binary packets are sent through UART0 TX on **GP0** at 921600 baud. USB CDC is reserved for readable diagnostics so text never corrupts the binary packet stream.

## Build

```bash
export PICO_SDK_PATH=/path/to/pico-sdk
cmake -S projects/rp2040_pico -B build/rp2040 -DPICO_BOARD=pico
cmake --build build/rp2040 -j
```

Copy the generated UF2 to the Pico boot drive or flash with your preferred debugger.

## Boot profile

The firmware first reads the ADS1299 ID, then configures 250 SPS / gain 24 / internal calibration test input on all eight channels. Real-electrode capture is intentionally a later validation step.

## Runtime integrity

DRDY is counted in an interrupt. If more than one DRDY edge accumulates before the main loop services it, the firmware increments a drop counter and sets packet flag bit 0. The common packet sequence number gives the host a second independent loss check.
