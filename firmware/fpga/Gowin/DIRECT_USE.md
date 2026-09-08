# Gowin direct-use integration

Both `GW1N_Reference` and `GW2A_Reference` can use the common ADS1299 datapath without vendor-specific HDL primitives.

Add these Verilog sources to the Gowin project:

```text
../common/rtl/ads1299_capture_core.v
../common/rtl/ads1299_frame_unpack.v
../common/rtl/ads1299_frame_to_bytes.v
```

Keep the selected reference project's existing device and package constraints. Bind `ads_drdy_n`, `ads_miso`, `ads_cs_n`, `ads_sclk`, and `ads_mosi` to the PCB pins used by the ADS1299 core board.

`CLOCK_DIV` determines SCLK using:

```text
fSCLK = fSYS / (2 * CLOCK_DIV)
```

After the MCU/soft CPU configures the ADS1299 and enables RDATAC, assert `enable`. Each DRDY falling edge triggers a 216-bit capture. `frame_valid` can feed `ads1299_frame_to_bytes` and then a FIFO/UART/USB/Ethernet block.

For Tang/Gowin boards that share the same FPGA family, keep board pin assignments in their own reference folder; do not duplicate the ADS1299 capture logic.

Before electrode testing, verify the common testbench and run ADS1299 internal test mode.
