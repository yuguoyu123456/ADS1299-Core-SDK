# Microchip FPGA direct-use integration

Use `PolarFire_Reference` as the device shell and add the portable ADS1299 RTL from `../common/rtl` as HDL sources in Libero SoC.

```text
../common/rtl/ads1299_capture_core.v
../common/rtl/ads1299_frame_unpack.v
../common/rtl/ads1299_frame_to_bytes.v
```

Keep the PolarFire reference project's existing package, I/O standard and timing constraints. Bind DRDY/DOUT/CS/SCLK/DIN to the actual ADS1299 connector pins.

The shared block is the high-rate RDATAC path: ADS1299 configuration should be completed first by a soft processor, external MCU or another register-control block. A complete 216-bit conversion produces `frame_valid`; use the byte-stream adapter to feed a FIFO, fabric bridge or DMA path.

For 64 channels, eight parallel ADS1299 capture instances avoid daisy-chain bandwidth concentration when the PCB provides independent lanes. Always validate FPGA timing, digital I/O voltage and ADS1299 internal test signal before electrode measurements.
