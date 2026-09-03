# Arduino framework adapter — source added, not yet framework-verified

The Arduino adapter uses `SPI_MODE1`, MSB-first and manual CS. It is intended for fast bring-up on boards where the Arduino core exposes a suitable SPI peripheral.

Do not assume every small AVR board can sustain every ADS1299 data rate plus host serial output. Start at 250 SPS and document RAM/SPI/UART throughput per board.
