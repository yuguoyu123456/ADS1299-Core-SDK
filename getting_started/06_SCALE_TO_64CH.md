# 06 - Scale from 8 Channels to 64 Channels

Scale only after one ADS1299 is stable through ID, internal-test, input-short, 250 SPS EEG and PC packet validation.

## Target architecture

The intended 64-channel system is:

- 8 × ADS1299
- 8 channels per ADS1299
- total = 64 EEG channels

The preferred repository architecture does not require ADS1299 daisy-chain mode. Multiple devices may share SCLK/MOSI while using independent chip-selects and explicit synchronization signals, or be divided across multiple SPI/DMA lanes.

## Scale in stages

Do not jump directly from one device to eight.

Recommended sequence:

1. 1 device = 8 channels
2. 2 devices = 16 channels
3. 4 devices = 32 channels
4. 8 devices = 64 channels

At every stage repeat:

- ID read on every device;
- internal test on every device;
- input-short/noise capture;
- continuous acquisition;
- sequence/timestamp validation;
- transport stress test.

## Synchronization

A multi-device system must define how samples from different ADS1299 devices belong to the same sampling instant.

Design considerations include:

- common/controlled clock strategy;
- coordinated START/RESET behavior;
- DRDY relationship across devices;
- deterministic read order;
- timestamp assignment;
- inter-device skew measurement.

If tight inter-device skew is a primary requirement, use multiple synchronized SPI/DMA lanes or FPGA capture rather than assuming a long serial polling loop is simultaneous.

## Recommended logical sample structure

Each ADS1299 continues to produce its own canonical 49-byte SDK packet. One 64-channel sample can therefore be represented as a group of 8 validated device packets with:

- one shared host/acquisition epoch;
- known device index 0..7;
- per-device sequence information;
- device/sample timestamp relationship;
- 8 × 8 channels = 64 channels.

The Ethernet and USB helpers can aggregate eight canonical packets without altering the packet contents.

## Bandwidth accounting

Calculate transport bandwidth from actual encoded bytes, not just ADC payload bits.

For the canonical format:

`bytes_per_second = packet_size × sample_rate × device_count`

with `packet_size = 49` bytes for each ADS1299 device packet.

Then add framing and bus/protocol overhead for UART, BLE, USB, Ethernet or Wi-Fi.

Do not select UART/BLE for full-rate 64-channel raw streaming without first proving the complete bandwidth and back-pressure budget.

## Acquisition scheduling

For high-rate multi-device operation:

- keep DRDY ISR/event handling minimal;
- prefer DMA where it improves deterministic capture;
- use DMA-accessible buffers on cached MCUs;
- perform required cache maintenance on platforms where DMA is not hardware-coherent;
- queue completed frames into bounded rings;
- keep USB/network/radio stacks in lower-priority contexts.

Never block all acquisition because one transport consumer is slow.

## Buffer sizing

Choose ring depth from measured worst-case service latency, not an arbitrary large number.

Record:

- producer rate;
- worst observed consumer stall;
- queue high-water mark;
- overflow count;
- selected overflow policy.

If overflow occurs, report it explicitly. Do not silently overwrite samples unless the chosen policy intentionally does so and the loss counter is preserved.

## 64-channel validation checklist

Before calling the system 64-channel capable, demonstrate:

- all 8 device IDs are stable;
- all 64 internal-test channels decode correctly;
- all 64 shorted-input channels are captured and compared;
- device/channel order is fixed and documented;
- no unexplained sequence gaps under the target transport load;
- synchronization/skew is measured rather than assumed;
- transport throughput has margin at the target sample rate;
- reconnect/back-pressure behavior is known;
- long-duration capture is repeatable.

This is an engineering validation checklist, not a statement of medical-device certification or human-safety compliance.
