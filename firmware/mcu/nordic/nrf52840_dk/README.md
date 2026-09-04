# nRF52840 DK ADS1299 reference source

Status: **Reference source / build-pending**. Hardware validation pending.

Target: nRF52840 DK using nRF Connect SDK / nrfx. The shared reference app performs the full ADS1299 first-bring-up path and common packet streaming. Board work is intentionally limited to GPIO + microsecond timing first; hardware SPIM/UARTE and EasyDMA are a later performance upgrade.

Required signals: SCK, MISO, MOSI, CS, DRDY, RESET, PWDN, START and TX. SPI semantics are ADS1299 Mode 1, MSB first.
