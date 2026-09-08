# MCXA156 ADS1299 ready-to-run path

Reuse this folder's existing NXP board/port integration with `../_shared/nxp_ads1299_ready_app.*` and the core-driver profile layer.

Configure SPI mode 1 and DRDY falling-edge interrupt. The ISR should only signal acquisition; perform the exact 27-byte frame read in the acquisition context and queue completed frames.

Validate ID -> internal test -> input short -> EEG profile before adding DMA/transport/application load. Maintain sample and overflow counters.

See `../DIRECT_USE.md` for the complete workflow.
