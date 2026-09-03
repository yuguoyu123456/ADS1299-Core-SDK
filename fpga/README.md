# FPGA track

The FPGA implementation is a separate track from MCU ports.

Planned reference targets:

- AMD/Xilinx
- Intel/Altera
- Lattice

Planned reusable RTL blocks:

- `ads1299_spi_master.v`
- `ads1299_controller.v`
- `ads1299_frame_reader.v`
- `ads1299_channel_unpack.v`
- host packet formatter
- self-checking testbenches

The first acceptance target is a 216-bit ADS1299 frame reader synchronized to DRDY. RTL will remain `Planned` until simulation, synthesis, timing and real-board checks are completed.
