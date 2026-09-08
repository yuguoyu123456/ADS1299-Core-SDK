# Intel/Altera direct-use integration

Use `CycloneIV_Reference`, `CycloneV_Reference`, or `MAX10_Reference` as the board shell. Add the vendor-neutral ADS1299 RTL from `../common/rtl` to the Quartus project.

Required files:

```text
../common/rtl/ads1299_capture_core.v
../common/rtl/ads1299_frame_unpack.v
../common/rtl/ads1299_frame_to_bytes.v
```

In Quartus, add them as Verilog HDL files and keep the reference project's existing package/pin assignments. Constrain the actual system clock in the project's SDC and ensure there are no unconstrained primary clocks.

The capture block receives ADS1299 DRDY/DOUT and generates CS/SCLK. It assumes ADS1299 is already in RDATAC mode. `frame_valid` identifies one complete 27-byte sample frame.

For an SoC Cyclone V design, `ads1299_frame_to_bytes` can feed an Avalon-ST FIFO/bridge. For pure FPGA designs it can feed a UART, USB bridge, Ethernet MAC FIFO or custom DMA path.

For 8 ADS1299 / 64 channels, eight parallel capture instances are preferred when independent DOUT/CS are available. Verify I/O voltage, SCLK frequency and the ADS1299 internal test pattern before EEG acquisition.
