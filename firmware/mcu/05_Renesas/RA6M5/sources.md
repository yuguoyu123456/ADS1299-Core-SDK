# Reference provenance

Reviewed 2026-09-05. No vendor implementation or generated BSP copied into the
shipping adapter. Dependencies are installed externally; upstream headers retain
their own copyright and BSD-3-Clause notices.

- [Renesas FSP v6.5.0](https://github.com/renesas/fsp/tree/v6.5.0),
  commit `d3795123de40a9906e75ef03fb6fbf7aef430162`:
  `ra/fsp/inc/api/r_spi_api.h`, `r_ioport_api.h`, `r_transfer_api.h`,
  `ra/fsp/src/r_spi/r_spi.c`. Used for asynchronous completion, config ownership,
  API signatures and Close interrupt/peripheral shutdown semantics. These files
  identify BSD-3-Clause. The adapter is independently implemented.
- [Official EK-RA6M5 SPI example](https://github.com/renesas/ra-fsp-examples/tree/01a411dfc2e9808f489070c780a554a5bead6714/example_projects/ek_ra6m5/spi),
  `readme.txt` and `e2studio/configuration.xml`: source for MCU part selection
  and P410/P411/P412 SPI signals only. It is a DTC loopback example, not an
  ADS1299 application. No example source, binary, startup or linker is imported.
- [FSP release/tool requirements](https://github.com/renesas/fsp/releases/tag/v6.5.0):
  e2 studio minimum 2026-04.2; Arm GNU 13.2 listed for this release. Local
  GCC 9.2.1 emulator-test compilation is not an endorsed FSP board toolchain.

The common ADS1299 Core remains the only chip-definition authority. Platform
references above do not define ADS1299 registers or commands.

## Additional compile-check dependencies

- [TinyUSB 0.18.0](https://github.com/hathach/tinyusb/tree/0.18.0), commit
  `86ad6e56c1700e85f1c5678607a762cfe3aa2f47`: `hw/bsp/ra/boards/ra6m5_ek`
  BSP configuration, family header include layout and `tools/get_deps.py`.
  Root MIT license, copyright 2018 hathach, inspected and retained in the
  ignored sparse test checkout. No TinyUSB USB middleware is built or copied
  into the adapter. Generated configuration is consumed without modification.
- [Renesas FSP 5.6.0](https://github.com/renesas/fsp/tree/v5.6.0), commit
  `edcc97d684b6f716728a60d7a6fea049d9870bd6`: the exact dependency pinned by
  TinyUSB 0.18.0. Real BSP/API headers and SPI/IOPORT implementation used for
  compile-only checks. BSD-3-Clause notices in upstream files retained.

Sparse local test dependencies are under `tests/.tools/`, excluded from source
distribution. No upstream implementation was pasted into shipping source.
