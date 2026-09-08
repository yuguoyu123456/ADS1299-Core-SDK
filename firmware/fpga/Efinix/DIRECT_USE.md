# Efinix direct-use integration

Use `Titanium_Reference` for the device/board shell and include the shared ADS1299 RTL from `../common/rtl`.

Required sources:

```text
../common/rtl/ads1299_capture_core.v
../common/rtl/ads1299_frame_unpack.v
../common/rtl/ads1299_frame_to_bytes.v
```

Add those files to the Efinity project as normal Verilog sources. Keep the Titanium reference project's existing pin, clock and device configuration; only bind the ADS1299 interface ports to real pins.

The capture core expects the ADS1299 to already be configured in RDATAC mode. `frame_valid` marks a complete 27-byte frame. Connect `frame_to_bytes` to an Efinity FIFO, DMA-facing bridge or another streaming block.

For 64 channels with eight ADS1299 devices, parallel capture instances are recommended when the PCB exposes eight DOUT/CS paths. Set `CLOCK_DIV` from the actual FPGA clock and verify the resulting SCLK before programming hardware.

Validation order: simulation -> ADS1299 internal test signal -> input-short noise -> electrode input.
