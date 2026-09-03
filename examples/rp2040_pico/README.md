# RP2040 / RP2350 Pico SDK example — source added, not yet toolchain-verified

Use SPI format compatible with ADS1299: CPOL=0, CPHA=1, MSB first. Configure CS as a normal GPIO because the common driver owns the full command transaction.

First bring-up target: read ID, then internal test signal at 250 SPS. Optimize SPI transfers/DMA only after correctness is established.
