# Rank 6 software-reference acceptance

Passed on 2026-09-05: real TI SDK 2.11.00.07, GCC 9.2.1, CMake 3.16.4/Ninja
compile and full reference ELF link. Undefined symbols: 0. Text/data/bss:
16240/276/76 bytes. See ../build.md for exact inputs and reproduction.

The original unbounded SPI waits are replaced by bounded FIFO/busy polling;
stale RX data returns an error. Long delays are split instead of truncated,
zero delay does not invoke TI's special maximum-delay behavior, GPIO masks
must be single-bit and unique, and DRDY cannot be driven by the write callback.
All chip registers and commands remain in the shared Core.

The sequential software-reference gate is passed; proceed to rank 7.
No runtime unit tests, flash operation, hardware ID/readback, acquisition or
long-duration run was performed. Reference status is retained conservatively.
