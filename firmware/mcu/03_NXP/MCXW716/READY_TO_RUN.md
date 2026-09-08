# MCXW716 ADS1299 ready-to-run path

Reuse this folder's existing NXP board/port integration with `../_shared/nxp_ads1299_ready_app.*` and the core-driver profile layer.

Configure SPI mode 1 and DRDY falling-edge GPIO interrupt. Keep radio work separate: the ISR signals acquisition, the acquisition context reads one exact 27-byte frame, and a queue feeds wireless/application tasks.

Validate ID -> internal test -> input short -> EEG profile before enabling radio traffic. Preserve sample sequence numbers and overflow statistics so wireless delays are visible.

See `../DIRECT_USE.md` for the complete workflow.
