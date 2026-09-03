# WCH CH32V307 EVT — ADS1299 complete reference project

Status: **Source-complete; CI build pending on this branch. Hardware validation pending.**

This project is the first WCH / QingKe RISC-V reference target in ADS1299-Core-SDK. It uses the WCH NoneOS Standard Peripheral Library through PlatformIO and shares the same portable ADS1299 driver and host packet format as the STM32F407, ESP32-S3 and RP2040 projects.

## Target

- MCU family: WCH CH32V30x
- Representative board: `ch32v307_evt`
- Representative MCU: CH32V307VCT6-class target
- Framework: WCH NoneOS SDK / Standard Peripheral Library
- Build system: PlatformIO
- Platform package: Community-PIO-CH32V, pinned to commit `499971abbee9d191c43132770be02fabfbf572cd`
- First validation profile: 250 SPS, gain 24, ADS1299 internal test source

The PlatformIO CH32V platform is a community packaging layer around WCH development materials. The ADS1299 application and port code in this repository are maintained here; WCH framework files are resolved as external build dependencies.

## Wiring

| ADS1299-Core | CH32V307 | Function |
|---|---|---|
| SCLK | PA5 | SPI1 SCK |
| DOUT / MISO | PA6 | SPI1 MISO |
| DIN / MOSI | PA7 | SPI1 MOSI |
| CS | PB6 | software chip-select |
| DRDY | PB7 | falling-edge EXTI |
| RESET | PB8 | active-low hardware reset |
| START | PB9 | start / synchronization control |
| DGND | GND | digital ground |

Host output uses USART1 TX on **PA9** at **921600 baud**. USART1 RX is configured on PA10 for future control commands but is not required for the first streaming demo.

## SPI configuration

The ADS1299 is driven as:

- master mode
- 8-bit transfers
- MSB first
- CPOL = 0
- CPHA = 1
- software-controlled CS
- conservative prescaler (`/32`) for bring-up

Do not change SPI mode during first hardware validation.

## Build

Install Python and PlatformIO, then from the repository root:

```bash
pio run -d firmware/mcu/wch/ch32v307_evt
```

PlatformIO will resolve the pinned CH32V platform and WCH NoneOS framework automatically.

For Windows + VS Code, opening this repository with the PlatformIO extension is sufficient for compilation. Flashing/debugging normally uses WCH-Link/WCH-LinkE or another programmer supported by the CH32V PlatformIO platform.

## Important CH32V307 memory-option note

Some CH32V30x devices support configurable Flash/SRAM partition options. The `ch32v307_evt` PlatformIO board profile has its own default memory configuration. **Do not copy memory-size overrides blindly.** The linker configuration and the physical option-byte configuration must agree or the MCU may fail to boot.

The ADS1299 reference firmware is small enough that the default profile is sufficient for this first project.

## Boot behavior

1. initialize WCH system clock / delay support;
2. configure GPIO, USART1, SPI1 and DRDY EXTI;
3. hardware-reset ADS1299;
4. issue `SDATAC`;
5. read and print the ADS1299 ID register;
6. configure 250 SPS and read CONFIG1 back;
7. route all eight channels to the internal test source at gain 24;
8. enter `RDATAC` and start conversion;
9. on every DRDY edge, read one complete 27-byte frame;
10. convert channels through the common ADS1299 driver;
11. emit the repository common binary host packet at 921600 baud.

The first profile intentionally does **not** use human electrodes. Internal-test-first bring-up isolates digital interface and configuration problems before electrode/contact/noise problems are introduced.

## Packet timing

The first CH32V307 example uses a sample-timeline timestamp derived from the fixed 250 SPS stream (`4000 µs` per DRDY period). This keeps the first implementation independent of CH32V30x timer configuration while still allowing the host to detect sample gaps together with the sequence counter.

A later performance revision can replace this with a free-running hardware timer without changing the packet format.

## Validation required before status upgrades

### Compiles

- clean GitHub Actions build from a fresh environment

### Bench-tested

- ADS1299 ID read captured from real hardware
- CONFIG1 write/readback confirmed
- eight-channel internal test waveform captured
- DRDY interrupt verified at 250 SPS
- packet decoder shows continuous sequence numbers
- no framing/CRC errors during a meaningful bench run

### 24h-tested

- continuous 250 SPS acquisition for at least 24 hours
- packet-loss count recorded
- CRC/framing error count recorded
- power and thermal behavior documented

Until physical evidence exists, this project must remain **Pre-hardware-validation** even when CI is green.
