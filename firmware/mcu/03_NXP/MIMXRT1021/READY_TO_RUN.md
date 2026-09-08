# MIMXRT1021 ADS1299 ready-to-run path

Reuse this folder's existing NXP board/port integration with `../_shared/nxp_ads1299_ready_app.*` and the core-driver profile layer.

Configure LPSPI for SPI mode 1 and DRDY falling-edge GPIO interrupt. Use the ISR only to signal acquisition; read one exact 27-byte frame in the acquisition context and queue completed frames.

Validate ID -> internal test -> input short -> EEG profile first. Add DMA only after the basic path is stable and keep transport behind a ring buffer.

See `../DIRECT_USE.md` for the complete workflow.
