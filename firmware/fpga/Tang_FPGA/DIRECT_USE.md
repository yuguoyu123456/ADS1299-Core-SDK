# Tang FPGA direct-use integration

`GW_Series_Reference` can directly reuse the shared ADS1299 pure-Verilog datapath in `../common/rtl`.

Add:

```text
../common/rtl/ads1299_capture_core.v
../common/rtl/ads1299_frame_unpack.v
../common/rtl/ads1299_frame_to_bytes.v
```

Tang boards commonly use Gowin-family FPGA devices, so keep the board-specific device/package/pin setup from `GW_Series_Reference` and only add the shared acquisition sources.

Connect `ads_drdy_n`, `ads_miso`, `ads_cs_n`, `ads_sclk`, and `ads_mosi` to the ADS1299 interface. The ADS1299 should be configured and placed into RDATAC before asserting `enable`.

`frame_valid` marks one 27-byte conversion frame. Send it to `ads1299_frame_to_bytes` and then into the board's USB/UART/FIFO path, or unpack channels in FPGA fabric with `ads1299_frame_unpack`.

For eight ADS1299 devices, use eight parallel capture engines if the connector exposes independent DOUT/CS signals. Start validation with the common simulation testbench and the ADS1299 internal test source before using real EEG electrodes.
