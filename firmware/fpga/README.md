# ADS1299 FPGA interface layer

Ranks 71-85 are FPGA/CPLD interfaces and remain separate from MCU ports.

## Verilog RTL sources

The FPGA reference packages contain **Verilog RTL (`.v`) source code** for the ADS1299 digital acquisition path. This is a hardware-description implementation, separate from the MCU C driver.

Each reference package includes the same core Verilog structure:

```text
rtl/
├── ads1299_spi_master.v      # ADS1299 SPI Mode-1 byte engine
├── ads1299_controller.v      # ADS1299 transaction / acquisition controller
└── frame_parser.v            # 4/6/8-channel ADS1299 frame parser

tb/
└── simulation_tb.v           # self-checking Verilog testbench
```

The Verilog RTL is intentionally vendor-neutral so the same acquisition logic can be reused across supported FPGA families while vendor-specific constraints and integration files remain inside each platform package.

Presence of Verilog source does not by itself imply synthesis, timing closure, board-level verification or hardware validation. Those claims require explicit validation evidence.

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
