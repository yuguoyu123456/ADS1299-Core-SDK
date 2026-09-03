# Platform ports

All ports adapt the same `drivers/ads1299` core. A port folder may exist while still being `Planned`; see `docs/SUPPORT_MATRIX.md` for verification status.

Required adapter functions: SPI full-duplex transfer, CS, optional RESET/START/DRDY, and microsecond delay.
