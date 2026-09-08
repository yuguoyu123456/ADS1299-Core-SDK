# Multi-ADS1299 Hardware: 16 / 32 / 64 Channels

This folder defines the project-default hardware architecture for scaling one validated ADS1299 core module to 2, 4 or 8 devices.

## Target sizes

- 1 × ADS1299 = 8 channels
- 2 × ADS1299 = 16 channels
- 4 × ADS1299 = 32 channels
- 8 × ADS1299 = 64 channels

Scale in that order. Do not debug eight boards at once if the one-device path has not already passed ID, internal-test and input-short validation.

## Project-default control architecture

The normal architecture is explicit multi-device control rather than requiring ADS1299 daisy-chain mode.

Typical shared signals:

- SCLK
- MOSI / DIN
- START when one synchronized start source is used
- RESET when one synchronized reset source is used
- common/external clock when the selected clock topology requires it

Per-device signals:

- CS0..CS7
- DRDY0..DRDY7, or another deliberately validated DRDY aggregation strategy

MISO/DOUT can be shared only when electrical bus ownership is guaranteed by chip select and the complete fan-out/tri-state behavior has been validated. Multiple SPI peripherals/lanes may instead be used for higher deterministic throughput.

## Recommended acquisition architectures

### Architecture A - one shared SPI bus

Useful for lower sample rates and simple systems. The host services each device in a deterministic order after the synchronized sampling event.

Advantages:

- simplest wiring;
- few host SPI resources.

Tradeoffs:

- sequential readout creates inter-device read latency;
- bus time grows with device count;
- tighter synchronization requirements may exceed what one serial bus can comfortably provide at higher rates.

### Architecture B - multiple SPI/DMA lanes

Split devices across two or more SPI peripherals.

Advantages:

- lower per-lane service time;
- parallel DMA;
- reduced readout skew;
- more margin for 64-channel operation.

### Architecture C - FPGA capture

Use the repository's `firmware/fpga/` capture core for strict deterministic multi-device sampling/readout.

Advantages:

- parallel deterministic capture;
- good fit for 64 channels and tight skew requirements;
- MCU can focus on packetization/USB/network control.

Do not claim one architecture is synchronized enough without measuring the actual inter-device timing on hardware.

## Files

- `SPI_FANOUT_AND_CS.md` — bus ownership, fan-out and CS rules.
- `CLOCK_START_DRDY_SYNC.md` — synchronization architecture and skew measurement.
- `BACKPLANE_POWER_SIGNAL.md` — 8-module carrier/backplane rules.
- `CHANNEL_MAP_64CH.md` — deterministic device/channel numbering.
- `THROUGHPUT_BUDGET.md` — raw frame and canonical-packet bandwidth accounting.
- `VALIDATION_CHECKLIST.md` — staged 16/32/64-channel acceptance tests.

## Firmware pairing

Use:

- `firmware/core_driver/ads1299/`
- `firmware/mcu/_shared/reference_app/`
- `firmware/fpga/`
- `firmware/common/data_packet/`
- `firmware/transport/`

The hardware and firmware must share the same device index, channel order, clock/start strategy and packet grouping.
