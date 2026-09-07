# SAME54 bounded polling adapter

Original hardware code using official Microchip DFP register definitions. No
ADS1299 chip behavior is implemented here. No Harmony SPI implementation is
copied or wrapped: the selected reference PLIB has unbounded synchronization
waits, and its asynchronous interface would need separate cancellation handling.

The board must configure SERCOM clocks/mux, disable its NVIC IRQs and DMA, and
initialize safe GPIO directions before `ads1299_same54_make_hal()`. Use static
zero-initialized context; no sharing with a SPI PLIB, ISR, DMA or another task.
This profile selects PAD0 output, PAD1 clock, PAD3 input, 8-bit MSB, Mode 1.

Each synchronization/DRE/RXC/TXC wait has a finite `poll_limit`. This is an
iteration budget, not a calibrated microsecond timeout. The baud divisor rounds
up so generated SCK does not exceed the requested rate; unrepresentable slow
rates are rejected. Null TX sends zeros, null RX discards received bytes.
The adapter waits for the final shift-register TXC, not just transmit-buffer
availability. CS belongs entirely to Core's separate GPIO callback.

Unexpected RX, overflow, error flag or timeout latches a fault and requests
peripheral disable without an unbounded cleanup wait. Reinitialize/reset the
board before reuse; do not simply clear `fault`. No caller buffer is owned by
an ISR/DMA. On any failure discard the whole received transaction, including
earlier bytes already written. GPIO CS deassertion remains possible after faults.

The board reference uses the externally installed Harmony clock/NVM/PORT
initializers. Their clock-startup waits are **not** bounded by this adapter.
Only adapter waits are covered by the fault tests. No sustained sample-rate,
multi-device, DMA, USB or hardware-acquisition performance is claimed.
