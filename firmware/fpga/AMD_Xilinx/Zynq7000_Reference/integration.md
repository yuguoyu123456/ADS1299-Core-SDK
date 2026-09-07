# Integration

1. Create a clean Vivado project for the exact board and part.
2. Add the three files under `rtl/` and select the intended top-level wrapper.
3. Copy the official board constraints; map clock and ADS1299 pins explicitly.
4. Constrain generated SCLK and both input/output timing paths.
5. Feed opaque command/register bytes from the shared Core or host sequencer.
6. Put parsed frames into an async FIFO before USB, PCIe, Ethernet or DMA logic.
7. Run `tb/simulation_tb.v`, synthesis and timing analysis before hardware use.
8. Validate ID read and the internal test signal on real ADS1299 hardware.

Do not commit tool caches, generated IP output, bitstreams or timing databases.
