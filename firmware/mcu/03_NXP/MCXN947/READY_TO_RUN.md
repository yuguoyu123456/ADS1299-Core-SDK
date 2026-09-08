# MCXN947 ADS1299 ready-to-run path

Reuse this folder's existing NXP board/port integration with `../_shared/nxp_ads1299_ready_app.*` and the core-driver profile layer.

Configure SPI mode 1 and DRDY falling-edge interrupt. Use the ISR only to signal a high-priority acquisition context, perform one exact 27-byte read per DRDY, then queue frames for transport/DSP.

For high-throughput designs, add DMA after the basic path is proven. Keep cache/DMA coherency and queue overflow accounting explicit.

Validate ID -> internal test -> input short -> EEG profile first. See `../DIRECT_USE.md` for the complete workflow.
