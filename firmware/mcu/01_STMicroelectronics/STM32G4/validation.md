# Validation

Current status: **Reference**

Allowed lifecycle states: `Planned`, `Reference`, `Example`, `Compatible`,
`Compiles`, `Bench-tested`, `24h-tested`.

STM32G474 reference linked with genuine device startup and CubeG4 1.5.2 linker on 2026-09-06; modeled SPI tests passed. See build.md. No board execution or CubeIDE build claimed.

To advance status, attach exact SDK/compiler versions, the clean command and
log. `Bench-tested` additionally requires a real ADS1299 ID read, internal-test
waveform and packet integrity evidence. `24h-tested` requires loss/CRC/error
counts from a continuous 24-hour run.

---

## Standardized SDK maintenance status

The historical lifecycle label above is preserved. For the current standardized SDK program, STM32G4 is additionally classified as:

**INTEGRATION CANDIDATE-COMPLETE**

This means the model folder now contains the essential software/integration pieces needed for a candidate-complete beginner path, while target-build and physical-board evidence remain separate gates.

### Present

- NUCLEO-G474RE reference-board-first documentation.
- `board/board_config.h` as the obvious board/config layer.
- STM32G4 SPI/GPIO/DRDY portable port sources.
- Progressive beginner example covering ID/probe, internal test, input short, 250-SPS/gain-24 EEG setup, streaming and finite clean stop.
- Shared canonical packet encoder integration.
- Legacy debugger example preserved.
- CMSIS/reference-image experimental path preserved separately.
- Host/integration smoke-test sources and build recipes present.

### Not yet claimed

- standardized beginner project successfully built in STM32CubeIDE: **NOT RECORDED**;
- physical NUCLEO-G474RE + ADS1299 execution: **NOT BOARD-VERIFIED**;
- sustained EXTI/DMA acquisition with measured queue overflow/loss: **NOT VERIFIED**;
- multi-ADS1299 shared-SPI operation: **NOT VERIFIED**;
- 8-device / 64-channel operation: **NOT VERIFIED**;
- electrical safety, EMC, production or regulatory readiness: **NOT CLAIMED**.

### Evidence required for the next gates

For **BUILD-VERIFIED**, retain the exact STM32CubeG4/CubeIDE version, compiler version, target/reference configuration, clean build command or reproducible IDE procedure, and successful build log.

For **BOARD-VERIFIED**, additionally retain the exact board revision/wiring, ADS1299 variant and power configuration, successful ID/probe result, internal-test result, input-short result, 250-SPS frame acquisition evidence and packet integrity evidence.

The existence of host tests, modeled SPI tests or a linker/reference-image experiment must never be promoted to physical BOARD-VERIFIED evidence.
