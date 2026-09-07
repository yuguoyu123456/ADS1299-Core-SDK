# Rank 8 acceptance record — 2026-09-05

Current lifecycle: **Reference**. Sequential software reference-build gate: passed.

| Check | Evidence | Boundary |
|---|---|---|
| Adapter behavior | run_adapter_tests.py PASS with FSP 5.6 and 6.5 API headers | ARM emulator, fake BSP and peripheral callbacks |
| Real BSP object compilation | compile_bsp_headers.py PASS, 11 objects, GCC 9.2.1 | FSP 5.6 + TinyUSB 0.18 configuration; no linking |
| FSP 6.5 real BSP | Attempt failed: missing bsp_linker_info.h | Older TinyUSB config is insufficient; no fabricated replacement |
| Layer/catalog validators | MCU 85, global ranks 1..100, transport passed | Structural checks only |
| Full reference ELF | PASS, 25 sources, undefined symbols 0 | FSP 5.6, original SPI1 IRQ/timer/GPIO binding; physical routing unverified |
| ADS1299 hardware | Not performed | No ID, waveform, measured timing, packet loss or 24-hour evidence |

The existing test_spi/test_reset/test_register/test_frame C files are integration
test entry points, not evidence that they have run on a device. The example's
latest-frame variable is a debugger snapshot, not a continuous output transport.

The first two rows alone were insufficient. The subsequent full reference link
clears the sequential software gate, not the hardware gate. See ../build.md for
reproducible commands and dependencies.
