# LPC55S69 ADS1299 ready-to-run path

Reuse this folder's existing NXP board/port integration with `../_shared/nxp_ads1299_ready_app.*` and the core-driver profile layer.

Configure SPI/FLEXCOMM for SPI mode 1 and DRDY falling-edge interrupt. Keep the ISR short, perform the 27-byte transfer in the acquisition context, then queue frames for application/USB/security-domain processing.

Validate ID -> internal test -> input short -> EEG profile before enabling TrustZone/application complexity. Keep peripheral ownership and buffers in the intended security domain.

See `../DIRECT_USE.md` for the full workflow.
