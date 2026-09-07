# Renesas FSP hardware adapter

This original adapter targets the `r_spi` and `r_ioport` API contracts in FSP
6.5.0. It is not a generated BSP or a standalone e2 studio project.

`ads1299_fsp_open()` opens a dedicated SPI instance, copies its generated
configuration into persistent context storage, and attaches a private completion
callback. It does not modify generated files or require `callbackSet()`.
IOPORT, muxing, clocks and interrupt routing remain owned by the board project.

## Required configuration

- SPI master, Mode 1 (idle low, sample even edge), MSB first, initially 1 MHz.
- Use interrupt-driven `r_spi`, all RXI/TXI/TEI/ERI vectors enabled. No DTC/DMAC.
- No other task, ISR or device may use this SPI instance. Never call the adapter
  with interrupts masked or from an ISR that prevents SPI interrupt completion.
- Physical CS is GPIO, not an SSL peripheral output. Keep CS asserted throughout
  Core transactions, including commands spanning several transfers.
- Open IOPORT first: CS/RESET/PWDN output high, START output low, DRDY input.
  Provide valid distinct package pins; this adapter cannot validate board routing.
- Supply an independently advancing, free-running **microsecond** clock returning
  `uint32_t` (wrap modulo 2^32), and a delay callback providing at least the
  requested duration. A loop counter is not a calibrated timer. A tick incremented
  by the same potentially blocked SPI IRQ is not suitable.
- Set timeout, e.g. 10000 us per <=32-byte chunk at 1 MHz. This bounds a wait only
  when the supplied timer works; no software can prove that external contract.
  An independent 100000-poll failsafe also terminates if the clock freezes. This
  count is not a calibrated time limit and can fail earlier on a different CPU.

## Buffer and fault ownership

The context must be static, zero-initialized before first use. Scratch buffers
are persistent, including after failed close. Caller buffers are copied only
after a completion callback. Null TX sends zeros; null RX discards input.
SPI never changes CS. A multi-chunk failure can leave earlier RX chunks written;
the caller must discard the entire transaction on any error.

Timeout, callback error or submission error closes SPI and latches a fault.
FSP 6.5.0 `R_SPI_Close` disables IRQs and the peripheral. Reopen explicitly only
after close succeeds, then reinitialize the ADS1299 through Core. If close fails,
keep context alive and do not reuse it. GPIO failures are latched; DRDY read returns
-1 instead of falsely reporting readiness. Output writes still permit safe CS
deassertion after a fault. Caller must close the adapter on a GPIO failure.

No DMA throughput or acquisition-loss guarantee is made. See `../build.md` for
the exact distinction between executed adapter tests and a full board build.
