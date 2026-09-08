# AMD/Xilinx direct-use integration

Use the existing Artix-7, Kintex-7, Zynq-7000 or Zynq UltraScale+ reference project as the board shell, and add the vendor-neutral ADS1299 capture RTL from `../common/rtl`.

## Files to add

From inside an AMD/Xilinx reference project, add:

```text
../../common/rtl/ads1299_capture_core.v
../../common/rtl/ads1299_frame_unpack.v
../../common/rtl/ads1299_frame_to_bytes.v
```

For simulation also add:

```text
../../common/tb/ads1299_capture_core_tb.v
```

## Vivado Tcl example

Run from the selected reference-project directory or adjust paths as needed:

```tcl
read_verilog ../../common/rtl/ads1299_capture_core.v
read_verilog ../../common/rtl/ads1299_frame_unpack.v
read_verilog ../../common/rtl/ads1299_frame_to_bytes.v
```

Keep the board-specific `constraints/pins.xdc` file and bind these top-level signals to physical pins:

```text
ads_drdy_n
ads_miso
ads_cs_n
ads_sclk
ads_mosi
```

Add a real clock constraint for the FPGA system clock, for example:

```tcl
create_clock -name sys_clk -period 10.000 [get_ports clk]
```

The 10 ns example corresponds to 100 MHz; replace it with the actual board oscillator period.

## Recommended topology

For one ADS1299, instantiate one `ads1299_capture_core` and connect `frame_valid` to `ads1299_frame_to_bytes` or a FIFO.

For eight ADS1299 devices / 64 EEG channels, use eight capture instances when the PCB exposes independent DOUT/CS lines. This is preferable on FPGA because all eight 27-byte frames can be acquired in parallel and aligned on DRDY.

## Vivado checks before programming

1. Confirm `CLOCK_DIV` keeps ADS1299 SCLK inside the device timing limit.
2. Confirm `ads_drdy_n` is treated as asynchronous to the FPGA system clock; the common core already contains a two-flop synchronizer.
3. Check that the selected I/O bank voltage matches the ADS1299 digital I/O supply.
4. Run timing analysis with no unconstrained system clocks.
5. Run `../../common/tb/ads1299_capture_core_tb.v` before hardware testing.
6. On hardware, first use ADS1299 internal test signal, then input-short noise, then electrodes.

The existing board-specific reference RTL remains valid; this common path is intended as the production acquisition datapath that can be shared across all AMD/Xilinx families.
