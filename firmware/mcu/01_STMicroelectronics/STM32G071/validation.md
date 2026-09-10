# Validation

Current status: **INTEGRATION CANDIDATE-COMPLETE**

This means the STM32G071 model folder now has a concrete reference board/config layer, STM32 HAL/portable ADS1299 port sources, progressive beginner example, canonical packet integration and host-test recipe. It does **not** mean target build or hardware validation has been completed.

## Evidence currently present

- Reference board defined: NUCLEO-G071RB (MB1360, STM32G071RBT6).
- Board/config entry present: `board/board_config.h`.
- Port sources present: SPI/GPIO/DRDY adapter.
- Progressive flow present: probe/ID -> internal test -> input short -> 250-SPS gain-24 EEG -> canonical stream -> clean stop.
- Shared canonical packet integration present.
- Host smoke/integration test sources and `Makefile.host` recipe present.
- Small-MCU bounded-buffer/overflow guidance present.

## Evidence not yet recorded

- Host test execution PASS: **NO**.
- STM32CubeIDE target clean compile/link: **NO**.
- BUILD-VERIFIED status: **NO**.
- NUCLEO-G071RB + physical ADS1299 run: **NO**.
- BOARD-VERIFIED status: **NO**.
- Sustained loss-accounted acquisition: **NO**.
- DMA/EXTI production acquisition path: **NO**.
- Multi-ADS1299 model validation: **NO**.
- 8x ADS1299 / 64-channel validation: **NO**.
- 24-hour, EMC, electrical-safety, production-readiness or regulatory evidence: **NO**.

## Next validation ladder

1. Execute the model host smoke tests and record the exact compiler/version and result.
2. Create the documented NUCLEO-G071RB Cube project and record a clean target compile/link.
3. Run probe/ID and internal-test on physical ADS1299 hardware.
4. Run input-short and finite 250-SPS EEG capture.
5. Validate canonical VCP packet streaming with sequence/CRC accounting.
6. Introduce EXTI/bounded buffering for sustained capture and measure overflow/loss.
7. Only after those steps evaluate multi-device scalability.

No unperformed validation is implied by source-file completeness alone.
