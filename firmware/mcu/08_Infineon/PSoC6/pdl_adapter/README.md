# PSoC6 PDL polling adapter

Original hardware-only adapter, not a copied ADS1299 driver or HAL SPI wrapper.
Core remains the sole owner of converter commands, registers and frame parsing.

The caller owns a dedicated SCB on CM4, clock divider/HSIOM configuration, GPIO
directions and safe startup levels. No CM0+ task, HAL SPI instance, ISR or DMA may
share it. Use static zero-initialized context and a valid, initially disabled SCB.
Configure physical CS as GPIO; do not route peripheral SS to the ADS1299.

Required SPI configuration: master, Motorola, CPHA1/CPOL0 (Mode 1), 8-bit TX/RX,
MSB first, no free-running clock or late MISO sampling, no interrupt masks.
Oversample must be 4..16. The board selects a known SCB clock so
`SCK = SCB clock / oversample`, initially 1 MHz. No clock setup is hidden here.

Nonblocking PDL Write/Read/FIFO-status APIs are polled with a finite iteration
budget for each wait. This is not a calibrated microsecond deadline. Null TX
sends zero; null RX discards data. RX overflow, stale RX, timeout or missing RX
data latches a fault and disables the SCB. Discard the entire transaction on
error, including earlier bytes. Final TX completion checks both FIFO and shifter.
No SPI function changes CS. GPIO writes still allow deasserting CS after faults.

Before retrying, reset/reinitialize the board and converter; do not just clear
the fault flag. Delay callback must provide actual minimum microsecond delays.
Pin numbers must be 0..7 and GPIO ports valid for the exact device; the adapter
cannot validate physical header routing or shared-board circuitry.

See ../build.md: real-header object compilation passed, but behavioral tests and
a complete CM0+/CM4 reference image are still pending. This is not Bench-tested.
