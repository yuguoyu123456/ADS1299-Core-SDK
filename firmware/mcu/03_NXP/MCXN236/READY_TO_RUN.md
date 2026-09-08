# MCXN236 ADS1299 ready-to-run path

Reuse this folder's existing NXP board/port integration with `../_shared/nxp_ads1299_ready_app.*` and the core-driver profile layer.

Configure SPI mode 1 and DRDY falling-edge interrupt. Keep the ISR minimal, perform the 27-byte ADS1299 frame read in the acquisition context, and push frames into a queue/ring buffer.

Validate ID -> internal test -> input short -> EEG profile before enabling DMA or higher-level processing. Keep frame sequence and overflow counters visible.

See `../DIRECT_USE.md` for the full MCUXpresso workflow.
