# Lattice direct-use integration

Use `CertusNX_Reference` or `ECP5_Reference` as the board/device shell and add the common pure-Verilog ADS1299 datapath from `../common/rtl`.

```text
../common/rtl/ads1299_capture_core.v
../common/rtl/ads1299_frame_unpack.v
../common/rtl/ads1299_frame_to_bytes.v
```

The files do not instantiate Lattice primitives and can be synthesized in Radiant/Diamond or compatible open-source flows for supported devices. Keep package pins and clock constraints in the existing reference project.

Connect DRDY/DOUT from ADS1299 and return CS/SCLK/DIN. After external configuration enters RDATAC mode, assert `enable`. Each `frame_valid` pulse corresponds to one 27-byte ADS1299-8 conversion frame.

Use `ads1299_frame_to_bytes` for FIFO/USB/UART/Ethernet integration, or `ads1299_frame_unpack` if DSP will run inside FPGA fabric.

For multi-ADS1299 systems, use one capture block per independent DOUT/CS lane, then align frames with a shared sample counter. Verify clock timing and internal ADS1299 test signal before connecting electrodes.
