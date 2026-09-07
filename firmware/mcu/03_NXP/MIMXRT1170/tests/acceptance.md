# Sequential acceptance: rank 4

## Superseding scope decision and result (2026-09-05)

The user authorized a similar-board reference instead of requiring the locked
EVKB schematic. The **EVK-derived software reference gate is passed**: 11
repository translation units compile, the static library builds, and a complete
RAM-debug ELF links against real external NXP SDK runtime/startup/linker files.
`arm-none-eabi-nm -u` reports zero undefined symbols. Size: text 44672,
data 280, bss 2192 bytes. Proceed to rank 5 within this reference-only scope.

Reproduce with `compile_official_sdk.py` and `--link-reference-ram`; see build.md.
The catalog's EVKB entry remains Reference. EVKB wiring, hardware execution,
signal integrity and continuous acquisition are not passed or implied.
The original EVKB-specific gate below remains open, but no longer prevents
sequential software-reference maintenance after the user's scope decision.

The earlier fixture-only acceptance was insufficient to declare this platform
complete. Real SDK compilation has now been performed; this item remains open.

Verified on 2026-09-05: 10 translation units compile to Arm Cortex-M7 objects
using official NXP SDK 2.16.000 headers, external CMSIS Core headers and GCC
9.2.1. See `../build.md` for reproduction.

CMake 3.16.4/Ninja also built the real static library and example object.
No fixture board callback was supplied to disguise unresolved board integration.
The catalog remains Reference, not Compiles or Bench-tested.

Remaining before advancing the sequential maintenance gate:

- Verify physical DRDY/RESET/PWDN/START header pins against the board schematic.
- Supply and build the board initialization binding used by the example.
- Link a complete image against the consuming official SDK project.

Hardware acquisition and long-duration evidence are separate follow-up checks
and have not been performed. No connected EVKB/ADS1299 hardware is established
in this session.

NXP's official EVKB design-files download redirected to sign-in on 2026-09-05.
Do not substitute an EVK schematic or infer double-row header numbering.
Peripheral initialization and GPIO collision/range checks are now present;
this does not close the board-specific wiring and image-link gates above.
