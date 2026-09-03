# PC Tools

Computer-side tools for receiving, checking, recording and visualizing ADS1299-Core data.

## Start here

- `PROTOCOL.md` — common packet format used by MCU/FPGA projects.
- `python/` — packet decoding, recording, plotting and analysis-side utilities.

The PC layer is intentionally independent of the selected controller. STM32, ESP32, RP2040, FPGA and future platforms should emit the same logical packet format so one host toolchain can be reused.

Firmware-side packet encoding code lives under `firmware/common/data_packet/`; this folder focuses on the computer side.
