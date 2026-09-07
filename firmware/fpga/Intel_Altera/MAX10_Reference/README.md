# Intel FPGA MAX 10 ADS1299 FPGA reference

Global ecosystem rank: **77**. Status: **Reference**. No board, synthesis, timing or hardware claim is made.

- FPGA family: MAX 10
- Development tool baseline: Quartus Prime; pin the exact version in the consuming project
- Reference fabric clock: 100 MHz
- ADS1299 serial mode: SPI Mode 1, conservative 4 MHz target
- ADS1299 count: one in the reference; replicate CS/DRDY and budget timing for more
- Data path: byte transaction controller -> 15/21/27-byte parser -> application FIFO/DMA
- FIFO/DMA: integration boundary documented; no vendor FIFO/IP is bundled

The RTL transports opaque bytes and parses 4/6/8-channel frames. Register and
configuration policy stays in the shared ADS1299 Core or a host sequencer. The
constraint file is a template because no exact board revision is claimed.
