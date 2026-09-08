# Telink TLSR825x ADS1299 Ready-to-Run Guide

This file is additive; existing target files remain unchanged.

Configure the selected SPI peripheral for ADS1299 mode 1 (CPOL=0, CPHA=1), start conservatively around 2-4 MHz, and map CS/RESET/PWDN/START plus falling-edge DRDY through the existing callbacks. Validate `ID -> internal test -> input short -> 250 SPS EEG` before BLE application traffic is enabled.

For ADS1299-8, service one complete 27-byte RDATAC frame per DRDY and enqueue it into a bounded buffer before BLE/radio processing.

TLSR825x is wireless-oriented. Keep radio events and protocol-stack work outside the timing-critical DRDY path, and expose queue-overflow/sequence-gap counters so RF scheduling delays are measurable. First prove blocking SPI, then use non-blocking/DMA support only if the concrete SDK provides it. For 64-channel systems, use Telink mainly as a wireless gateway with FPGA or stronger-MCU capture.

See `../DIRECT_USE.md` and `../_shared/other_ads1299_ready_app.[ch]`.
