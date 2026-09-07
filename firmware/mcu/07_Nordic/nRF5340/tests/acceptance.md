# Rank 7 software acceptance

Passed 2026-09-05: fresh official Zephyr 3.7.1 build with pinned CMSIS and
Nordic HAL, hardware SPIM4 enabled, and zero undefined ELF symbols.
Reproducible clean-build command and limitations are in ../build.md.

The actual CMake application now uses the standardized Port instead of legacy
GPIO bit-banging. SPI and chip select are separated, EasyDMA buffers are in
RAM, GPIO readiness/errors are checked, and the minimal example publishes a
latest-frame snapshot. Packet/bit-banged UART legacy sources are not compiled.

Status: Compiles (local Zephyr build only). Proceed to rank 8.
No NCS 3.4 CI run, hardware acquisition, BLE/network-core image, UART/USB
stream validation, or long-duration verification is claimed.
