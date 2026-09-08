# MIMXRT1062 ADS1299 ready-to-run path

Reuse this folder's existing NXP board/port integration with `../_shared/nxp_ads1299_ready_app.*` and the core-driver profile layer.

Configure LPSPI for mode 1 and DRDY falling-edge GPIO interrupt. Keep the ISR minimal; perform one exact 27-byte ADS1299 transfer per DRDY in the acquisition context and queue frames for transport/DSP.

Validate ID -> internal test -> input short -> EEG profile before adding DMA. Keep cache/DMA buffer coherency and queue-overflow counters explicit.

See `../DIRECT_USE.md` for the complete workflow.
