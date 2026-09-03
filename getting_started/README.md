# Start Here

Welcome to ADS1299-Core-SDK.

If this is your first visit, use this learning path in order:

1. **Understand the hardware** — read the ADS1299 basics and the module pinout.
2. **Connect one ADS1299** — start with a single 8-channel device.
3. **Read the device ID** — prove SPI, CS, RESET and power are correct.
4. **Run the internal test signal** — verify the full ADS1299 digital acquisition path without electrodes.
5. **Stream eight channels to a PC** — use the common packet format and PC tools.
6. **Record a first EEG signal** — only after the internal-test and noise checks pass.
7. **Continue to EEG analysis and BCI** — use `eeg_analysis/` and `bci/`.

## Where should I click?

- I have an STM32 / ESP32 / RP2040 / other MCU → `firmware/mcu/`
- I use FPGA → `firmware/fpga/`
- I want the ADS1299 driver API → `firmware/core_driver/ads1299/`
- I want PC recording / plotting → `pc_tools/`
- I want EEG filtering / PSD / ERP → `eeg_analysis/`
- I want SSVEP / motor imagery / P300 → `bci/`
- I want multi-ADS1299 / 64-channel hardware guidance → `hardware/` and `docs/`

Every supported MCU folder is intended to become a complete reference project, not a partial code snippet.
