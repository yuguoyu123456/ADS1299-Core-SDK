# Allwinner R-Class ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Configure the selected real-time core's SPI for ADS1299 mode 1 (CPOL=0, CPHA=1), start around 2-4 MHz, and map CS/RESET/PWDN/START plus falling-edge DRDY through the existing callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` first.

Service one complete 27-byte ADS1299-8 frame per DRDY and transfer completed frames through a bounded queue or shared-memory channel to application-class processing.

Keep Linux/application-core/network activity off the real-time acquisition path. If an R-class core and application core coexist, reserve the real-time core for DRDY/SPI service and use explicit producer-consumer synchronization. For 8 x ADS1299 / 64 channels, multiple deterministic SPI/DMA lanes or the repository FPGA capture layer are preferred.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
