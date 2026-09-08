# LPC54628 ADS1299 ready-to-run path

Reuse this folder's existing NXP board/port integration with `../_shared/nxp_ads1299_ready_app.*` and the core-driver profile layer.

Configure the selected SPI/FLEXCOMM interface for SPI mode 1, DRDY as falling-edge interrupt, and keep the ISR minimal. Read one exact 27-byte frame per DRDY in the acquisition context, then queue it for transport.

Validate ID -> internal test -> input short -> EEG profile before adding DMA/USB/Ethernet/application load.

See `../DIRECT_USE.md` for the full MCUXpresso workflow.
