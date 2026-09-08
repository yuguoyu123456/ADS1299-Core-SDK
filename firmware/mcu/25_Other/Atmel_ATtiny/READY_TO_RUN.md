# ATtiny ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Use the concrete ATtiny hardware SPI/USI path only if it can meet ADS1299 mode-1 timing (CPOL=0, CPHA=1). Map CS/RESET/PWDN/START and falling-edge DRDY through the existing callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` before any application workload.

An ADS1299-8 frame is 27 bytes. Keep only the minimum static buffering required, avoid floating-point conversion and transmit raw bytes as soon as practical.

ATtiny devices are highly resource constrained and are not recommended as the main controller for full 8-channel continuous EEG unless the selected part has sufficient SPI/RAM/timing margin. They are not appropriate for direct 64-channel aggregation. Use this directory as a portability/control reference and move capture to a stronger MCU or FPGA when required.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
