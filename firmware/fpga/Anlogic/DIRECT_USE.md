# Anlogic direct-use integration

Use `EG4_Reference` as the board shell and add the shared RTL from `../common/rtl`:

```text
../common/rtl/ads1299_capture_core.v
../common/rtl/ads1299_frame_unpack.v
../common/rtl/ads1299_frame_to_bytes.v
```

The common core is pure synthesizable Verilog and does not depend on vendor primitives. Keep the existing EG4 pin/clock files for package-specific assignments.

## Minimum connection

```text
ADS1299 DRDY -> ads_drdy_n
ADS1299 DOUT -> ads_miso
ADS1299 CS   <- ads_cs_n
ADS1299 SCLK <- ads_sclk
ADS1299 DIN  <- ads_mosi
```

Configure ADS1299 with MCU/soft CPU first, enter `RDATAC`, then assert the capture core `enable` input. Every conversion produces one 216-bit frame and one `frame_valid` pulse.

For 8 ADS1299 devices, instantiate eight capture cores when independent DOUT/CS signals are available. Use a common acquisition timestamp/frame counter after all eight `frame_valid` signals are aligned.

Before hardware acquisition, verify system-clock timing, I/O voltage compatibility, SCLK divider, DRDY pin assignment and the internal ADS1299 test signal path.
