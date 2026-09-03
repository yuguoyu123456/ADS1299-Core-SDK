# Multi-ADS1299 SPI Architecture

> **Project default:** for multi-chip ADS1299 systems, this repository uses TI's standard/cascaded multi-device SPI architecture as the primary design. Daisy-chain mode is treated as an optional compatibility/advanced topic, not the mainstream reference architecture.

This design choice is deliberate. Modern MCUs and FPGAs usually have enough GPIO resources to keep each ADS1299 independently selectable, which makes device configuration, fault isolation, bring-up and long-term maintenance substantially easier.

## Default topology

For N ADS1299 devices:

- Share `SCLK` across devices.
- Share `DIN/MOSI` across devices.
- Share `DOUT/MISO` when using separate chip selects; the unselected ADS1299 places DOUT in high impedance.
- Give **every ADS1299 its own `CS`** line.
- Prefer a common clock source for synchronized sampling.
- Prefer a common hardware `START` signal when deterministic simultaneous start is required.
- During bring-up, keep `DRDY` available per device whenever GPIO resources permit; this makes synchronization faults and a failed device much easier to diagnose.
- `RESET` may be shared when the product intentionally resets the complete converter bank together; per-device reset is optional if the controller has abundant GPIO and fault isolation is a priority.

For eight ADS1299 devices, the shared serial bus itself needs `SCLK + MOSI + MISO + 8 x CS`; additional START/DRDY/RESET/CLK signals are chosen according to the board architecture.

## Why this is the project's primary architecture

1. **Independent register control** — every ADS1299 can be configured or inspected separately.
2. **Simple debugging** — one converter can be selected and tested without decoding a daisy-chain stream.
3. **Fault isolation** — a bad board/device can be disabled without losing access to the remaining devices.
4. **Straightforward software model** — the same portable `ads1299_t` driver handle can be instantiated once per converter.
5. **Natural scaling** — 8/16/24/32/64-channel systems use the same device-level API.
6. **Modern controllers have enough GPIO** — STM32, GD32, CH32, ESP32, nRF, NXP, TI MCU families and FPGAs commonly provide sufficient pins for separate chip selects.
7. **TI describes the cascaded/standard configuration as suitable for the majority of applications.**

## Software architecture

The MCU application should create one ADS1299 device instance per physical converter:

```c
ads1299_t ads[8];
```

Each instance shares the same SPI peripheral but owns a different chip-select callback/context.

Conceptually:

```text
                         +----------- ADS1299 #1 (CH1-8)
MCU/FPGA SCLK ----------+----------- ADS1299 #2 (CH9-16)
MCU/FPGA MOSI ----------+----------- ...
MCU/FPGA MISO <---------+----------- ADS1299 #8 (CH57-64)

GPIO CS1  --------------------------> ADS1299 #1 CS
GPIO CS2  --------------------------> ADS1299 #2 CS
...
GPIO CS8  --------------------------> ADS1299 #8 CS

SYNC START --------------------------> all ADS1299 START
COMMON CLK --------------------------> all ADS1299 CLK
```

Only one `CS` is asserted during a shared-bus transaction.

## Acquisition strategy

For synchronized EEG acquisition:

1. Configure every converter separately.
2. Verify every device ID and register dump separately.
3. Apply a common clock.
4. Start converters from a common START edge (or otherwise use a rigorously validated synchronization procedure).
5. On each sample period, read one 27-byte frame from every converter.
6. Preserve a device index and channel index so 64-channel ordering is deterministic.
7. The host packet layer should expose channel count/device count without requiring the PC software to know the MCU implementation.

At typical EEG rates such as 250, 500 and 1000 SPS, this sequential shared-SPI approach is practical on modern controllers when SPI speed and firmware scheduling are designed correctly. Very high aggregate sample rates must be checked against total bus bandwidth; high-rate systems may use multiple hardware SPI peripherals or parallel FPGA interfaces.

## 64-channel validation plan

For 8 x ADS1299 / 64 channels, do not declare hardware support until all of the following pass:

- all eight device IDs read correctly;
- every device can be independently selected and register-configured;
- common clock and START synchronization are checked;
- channel order is proven using deterministic internal/external test signals;
- all eight DRDY paths are checked during bring-up when available;
- 64-channel frame alignment is stable during long runs;
- packet-loss and sequence-counter statistics are recorded;
- 250/500/1000 SPS aggregate throughput is verified before higher rates;
- SPI signal integrity is checked at the physically farthest converter.

## Daisy-chain mode

ADS1299 daisy-chain mode remains a valid device feature and may be useful when GPIO count is extremely limited. It is retained as an advanced/compatibility topic, but it is **not the default topology used by ADS1299-Core-SDK reference designs**.

Primary device authority: Texas Instruments ADS1299 datasheet, Multiple Device Configuration section.
