# ADS1299 FPGA interface layer

Ranks 71-85 are FPGA/CPLD interfaces and remain separate from MCU ports.

| Rank | Vendor | Family | Package | Status |
|---:|---|---|---|---|
| 71 | AMD Xilinx | Zynq-7000 | `AMD_Xilinx/Zynq7000_Reference` | Reference |
| 72 | AMD Xilinx | Zynq UltraScale+ | `AMD_Xilinx/ZynqUltraScalePlus_Reference` | Reference |
| 73 | AMD Xilinx | Artix-7 | `AMD_Xilinx/Artix7_Reference` | Reference |
| 74 | AMD Xilinx | Kintex-7 | `AMD_Xilinx/Kintex7_Reference` | Reference |
| 75 | Intel FPGA | Cyclone V | `Intel_Altera/CycloneV_Reference` | Reference |
| 76 | Intel FPGA | Cyclone IV | `Intel_Altera/CycloneIV_Reference` | Reference |
| 77 | Intel FPGA | MAX 10 | `Intel_Altera/MAX10_Reference` | Reference |
| 78 | Lattice | ECP5 | `Lattice/ECP5_Reference` | Reference |
| 79 | Lattice | Certus-NX | `Lattice/CertusNX_Reference` | Reference |
| 80 | Microchip FPGA | PolarFire | `Microchip_FPGA/PolarFire_Reference` | Reference |
| 81 | Gowin | GW1N | `Gowin/GW1N_Reference` | Reference |
| 82 | Gowin | GW2A | `Gowin/GW2A_Reference` | Reference |
| 83 | Anlogic | EG4 | `Anlogic/EG4_Reference` | Reference |
| 84 | Tang FPGA | GW series | `Tang_FPGA/GW_Series_Reference` | Reference |
| 85 | Efinix | Titanium | `Efinix/Titanium_Reference` | Reference |

Every package contains the same vendor-neutral Mode-1 SPI byte engine,
transaction controller, 4/6/8-channel frame parser and self-checking
testbench. Only constraint syntax and integration documentation differ.

Register definitions and configuration policy remain in `core_driver/ads1299`.
No package claims simulation, synthesis, timing closure or hardware testing.

## Reusable direct-acquisition path

`common/` now provides the preferred shared high-rate datapath for new designs:

```text
common/rtl/ads1299_capture_core.v
common/rtl/ads1299_frame_unpack.v
common/rtl/ads1299_frame_to_bytes.v
common/tb/ads1299_capture_core_tb.v
```

This path is intentionally separate from the historical per-vendor reference copies. New projects should use the vendor folder for package/constraint/toolchain setup and use `common/rtl` for the ADS1299 sample datapath.

### Fastest route to a working design

1. Start from the matching vendor/family reference directory.
2. Add the three files from `common/rtl` to that project.
3. Connect ADS1299 `DRDY`, `DOUT`, `CS`, `SCLK`, `DIN`.
4. Configure ADS1299 with the MCU/core driver and enter `RDATAC`.
5. Assert the capture core `enable` input.
6. Use `frame_valid` + `frame_data` for FPGA DSP, or pass the frame through `ads1299_frame_to_bytes` into a FIFO/transport block.
7. Treat `missed_drdy` as a hard acquisition-integrity warning.

### 64-channel recommendation

For an 8 x ADS1299 system, the cleanest FPGA architecture is eight parallel capture engines when independent DOUT/CS lanes are available. Each engine captures one 27-byte frame, then a small frame aligner or shared sample counter combines them into a 64-channel sample set.

This avoids concentrating all traffic onto one daisy-chain lane and makes per-board diagnostics easier.

### Validation sequence

Use this order before real EEG acquisition:

```text
RTL simulation
  -> ADS1299 internal test signal
  -> input-short noise test
  -> electrode / signal-generator input
  -> long-duration missed-DRDY/overflow soak test
```

Each vendor directory now includes `DIRECT_USE.md` with the tool-specific entry path.
