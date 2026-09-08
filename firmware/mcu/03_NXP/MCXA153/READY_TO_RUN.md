# MCXA153 ADS1299 ready-to-run path

Reuse this folder's existing NXP board/port integration with `../_shared/nxp_ads1299_ready_app.*` and the core-driver profile layer.

Configure the available SPI interface for mode 1 and DRDY as falling-edge GPIO interrupt. Keep the ISR minimal; perform one exact 27-byte transfer per DRDY in the acquisition context and queue frames for transport.

Validate ID -> internal test -> input short -> EEG profile before enabling DMA or application processing.

See `../DIRECT_USE.md` for the complete MCUXpresso workflow.
