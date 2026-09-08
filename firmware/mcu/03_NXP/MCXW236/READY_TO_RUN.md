# MCXW236 ADS1299 ready-to-run path

Reuse this folder's existing NXP board/port integration with `../_shared/nxp_ads1299_ready_app.*` and the core-driver profile layer.

Configure SPI mode 1 and DRDY falling-edge GPIO interrupt. Keep radio/protocol processing out of the DRDY path: the ISR signals acquisition, the acquisition context reads exactly 27 bytes, and a queue feeds wireless tasks.

Validate ID -> internal test -> input short -> EEG profile before enabling wireless traffic. Preserve sample sequence numbers and queue-overflow counters.

See `../DIRECT_USE.md` for the full workflow.
