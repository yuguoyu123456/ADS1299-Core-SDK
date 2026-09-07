# Classic ESP32 queued hardware binding

Original SDK-owned hardware adapter, derived from the maintained C6 queue-lifetime
pattern, compiled separately with classic ESP32 headers. No ADS1299 driver copy.
The concrete board owns SPI3/VSPI, fixed 1 MHz Mode 1, DMA disabled and queue size 1.

Static context, one task, dedicated bus. Timeout values must be finite and nonzero.
On completion timeout, transaction and buffers remain valid for a late completion.
Never free/reset them or remove the device while still owned by IDF. Caller RX is
copied only on successful completion; no implicit recovery or requeue follows error.
GPIO output failures latch fault; subsequent DRDY reads return negative.

tests/fixtures are API doubles only, never production include files. The tests run
C logic on an ARM emulator, not ESP32 silicon. Actual IDF build status is in build.md.
