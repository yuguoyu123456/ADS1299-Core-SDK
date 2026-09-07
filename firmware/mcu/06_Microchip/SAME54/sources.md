# Rank 9 source audit — 2026-09-05

Work started after rank 8's complete software reference ELF passed. SAME54
remains **Reference**: no board compilation or hardware acquisition is claimed.

## Selected official reference

[Microchip Harmony SAM D5x/E5x CSP applications](https://github.com/Microchip-MPLAB-Harmony/csp_apps_sam_d5x_e5x/tree/4a38194d468e94d4710c31bbfd67fbfe9f5d2096),
commit `4a38194d468e94d4710c31bbfd67fbfe9f5d2096`, release notes v3.6.1.
Selected application: `apps/sercom/spi/master/spi_self_loopback_interrupt`.
The supplied application uses SERCOM6 on SAM E54 Xplained Pro.
Release notes list MPLAB X 6.25, MCC 5.6.3 and XC32 4.60; not installed or
verified as a local board-build toolchain in this maintenance step.

## License boundary

The root license is Microchip's software license V2.001, not unrestricted MIT
or BSD. It includes Microchip-device use restrictions and redistribution terms.
Do not relicense vendor source as this SDK's original code. The local reference
is an ignored test dependency under `tests/.tools/csp-reference`, with upstream
license/copyright retained. No implementation has been copied into shipping
Core or Port. Any future distributable must keep this dependency boundary clear.

## API findings to carry into implementation

- `SERCOM6_SPI_WriteRead()` in the selected interrupt example is asynchronous.
  Its boolean return is request acceptance, not completed frame data.
- The PLIB exposes `SERCOM6_SPI_IsBusy()`, `SERCOM6_SPI_CallbackRegister()` and
  `SERCOM6_SPI_IsTransmitterBusy()`. Review the implementation's TX completion
  behavior before deciding when it is safe to release GPIO CS.
- `SERCOM6_SPI_TransferSetup()` configures transfer format; the ADS1299 profile
  must select Mode 1, 8-bit, MSB first and an explicitly known source clock.
- Timeout recovery, pending interrupt/buffer lifetime and error handling still
  need implementation review. Do not wrap an unbounded vendor call with an outer
  timeout and claim it is bounded.
- Only hardware adaptation belongs here. All chip commands, registers and frame
  parsing remain in `firmware/core_driver/ads1299`.

## Subsequent implementation and build, 2026-09-06

An original DFP-based polling adapter now replaces the need to wrap the PLIB.
The official clock/NVM/PORT initializers are externally linked unchanged; they
remain subject to Microchip's license and are not relicensed or copied here.

GCC startup: [TinyUSB-pinned Microchip driver collection](https://github.com/hathach/microchip_driver/tree/9e8b37e307d8404033bb881623a113931e1edf27),
`same54/gcc/gcc/startup_same54.c`, copyright 2017 Microchip, Apache-2.0
notice reviewed and retained. It initializes C data/BSS and vectors, and does
not require XC32's proprietary runtime. It does not call C++ constructors.

External linker: [TinyUSB 0.18 SAME54 linker](https://github.com/hathach/tinyusb/blob/86ad6e56c1700e85f1c5678607a762cfe3aa2f47/hw/bsp/samd5x_e5x/boards/same54_xplained/same54p20a_flash.ld),
copyright 2019 Microchip, Apache-2.0. Correct SAME54P20A 1 MiB flash / 256 KiB
main RAM layout; unmodified and kept only under ignored tests/.tools.

The GNU reference link passed, with zero undefined symbols; modeled polling
fault tests also passed. This clears the sequential software reference gate,
not the hardware gate. The next platform is rank 10 PSoC6.
