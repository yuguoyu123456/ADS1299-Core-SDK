# SPI Fan-out and Independent Chip Select

The project-default multi-device architecture uses explicit device selection. Each ADS1299 must be addressable independently unless another validated topology is intentionally chosen.

## 1. Shared bus concept

A simple 2/4/8-device system may share:

- SCLK
- MOSI / DIN
- MISO / DOUT when bus ownership is electrically safe

and provide one chip select per ADS1299:

- CS0
- CS1
- ...
- CS7

Only one device may drive the shared MISO path at a time.

## 2. Host firmware contract

Firmware must map a fixed `device_index` to one physical CS line. Do not discover channel ordering from whichever device responds first.

Recommended table:

| Device index | CS | DRDY | SPI lane | Channels |
|---:|---|---|---|---|
| 0 | CS0 | DRDY0 | lane A | 1-8 |
| 1 | CS1 | DRDY1 | lane A/B | 9-16 |
| ... | ... | ... | ... | ... |
| 7 | CS7 | DRDY7 | lane N | 57-64 |

Populate the actual board-specific pin mapping in the board port, not in the portable ADS1299 driver.

## 3. Fan-out review

As the module count grows, evaluate:

- capacitive loading on SCLK/MOSI/START/RESET/CLK;
- trace stubs from the backplane to each module;
- edge overshoot/ringing;
- logic-level margin;
- return-current path;
- connector discontinuities;
- whether series damping near the driver is needed based on measurement/simulation.

Do not add arbitrary series resistors from another design without checking the actual topology.

## 4. MISO/DOUT ownership

If all DOUT pins share one MISO net, validate the ADS1299 output behavior for deselected devices against the datasheet and the real board.

If there is any risk of contention or the host architecture benefits from parallelism, use:

- separate MISO inputs;
- separate SPI peripherals;
- bus switches/multiplexing where justified;
- FPGA parallel capture.

## 5. Read-time budget

One ADS1299-8 continuous raw frame is 27 bytes. With N devices on one serial bus, each sampling period must accommodate:

- N × 27 bytes of SPI clocks;
- CS setup/hold/inter-device gaps;
- interrupt/DMA/software latency;
- margin for worst-case scheduling.

Do not design from average timing only. Measure worst-case service time under the intended USB/network/radio load.

## 6. DMA

DMA is recommended when it improves deterministic acquisition.

For cached MCUs:

- allocate DMA buffers in DMA-accessible memory;
- perform required cache clean/invalidate operations;
- avoid sharing a buffer with a lower-priority transport task while DMA still owns it.

For multi-lane systems, use one buffer/descriptor path per lane or another ownership model that cannot cross-corrupt frames.

## 7. Logic-analyzer validation

For 2, then 4, then 8 devices capture:

- all relevant DRDY lines;
- CS0..CSn;
- SCLK;
- MISO lane(s);
- START/CLK when synchronization is being validated.

Measure:

- DRDY-to-first-CS latency;
- time to read each 27-byte frame;
- total service window;
- variation/jitter across a long capture;
- whether any read overlaps the next sampling event.

## Acceptance gate

The SPI topology is acceptable only when every device is independently addressable, no output contention is observed, the complete frame set fits inside the sample-period budget with margin, and long-duration capture has no unexplained losses.
