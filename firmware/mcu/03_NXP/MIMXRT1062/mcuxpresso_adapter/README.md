# MCUXpresso adapter

Hardware-only bridge for the MIMXRT1060-EVKB Cortex-M7 project. The consuming
project initializes LPSPI1 and the five GPIOs, then calls
`ads1299_rt1062_make_hal()`. Use
`kLPSPI_MasterPcs0` as transfer flags; D10 remains
software-controlled physical CS.

Alternatively call `ads1299_rt1062_init()` after selecting clock roots. It
sets inactive control outputs, DRDY input and 8-bit Mode 1 MSB-first LPSPI.
The optional `board/evkb_reference.c` uses external BOARD_BootClockRUN, queries
the real clock tree, and applies the documented pad mux at 1 MHz SPI. Use this
only at startup, not while another subsystem owns the clocks/peripheral.
Context storage must persist. GPIO indices are 0..31, not masks; duplicates
are rejected, and the write callback never drives DRDY.

Physical PCS must not be muxed to LPSPI. Define a finite SPI_RETRY_TIMES while
compiling fsl_lpspi.c; the reference build uses 1000000 iterations (not a
wall-clock deadline). Reinitialize after an error; partial frames are invalid.
The example publishes latest-frame debugger globals, not lossless UART/USB
streaming. Set pad electrical parameters after measuring the actual PCB.
