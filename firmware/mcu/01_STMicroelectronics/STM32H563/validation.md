# Validation

Current status: **Planned**

Only a directory scaffold and generic callback contract exist for STM32H563.
No target SDK adapter, clean target compile/link, hardware test or 24h test is claimed.
The example requires a real board binding. Generic test source existence is not a test pass.

Next evidence: exact device/board/SDK -> SPI/GPIO binding -> full target link ->
ADS1299 ID/internal-test capture -> sustained acquisition and loss accounting.

---

## Current maintenance validation status

Maintenance state: **integration-candidate-complete**.

The historical `Planned` statement above is retained for catalog history, but it no longer describes the file inventory. The STM32H563 folder now contains a reference-board configuration, portable port adapter, progressive beginner example and host smoke-test recipe.

### Evidence present in repository

- NUCLEO-H563ZI reference board path documented.
- `board/board_config.h` is the intended single hardware-dependent repository configuration point.
- SPI/GPIO/DRDY portable port sources are present.
- Progressive example covers probe/ID, internal test, input short, 250-SPS gain-24 EEG, canonical packet streaming and clean STOP/SDATAC.
- Host test sources/recipe cover model-port semantics and shared-core integration boundaries.
- Legacy example remains preserved for compatibility.

### Validation claims intentionally NOT made

- Host smoke tests: **execution PASS not yet recorded**.
- STM32CubeH5 / NUCLEO-H563ZI target build: **not BUILD-VERIFIED**.
- Physical NUCLEO-H563ZI + ADS1299 execution: **not BOARD-VERIFIED**.
- Sustained EXTI/DMA acquisition: **not verified**.
- Queue overflow/loss accounting under sustained acquisition: **not verified**.
- Multi-ADS1299 or 8xADS1299/64-channel operation: **not verified**.
- Long-run, electrical safety, EMC, production or regulatory readiness: **not claimed**.

### Evidence required for the next status promotion

1. Run the host smoke-test recipe and record compiler/version plus PASS output.
2. Create or use the documented STM32CubeH5 NUCLEO-H563ZI project and complete a clean compile/link.
3. Record ADS1299 ID/probe output on physical hardware.
4. Capture internal-test and input-short results before electrode use.
5. Capture a bounded 250-SPS EEG stream and verify canonical packet decoding/CRC on the host.
6. For sustained/DMA operation, record queue high-water mark, overflow count and transfer-loss accounting before claiming that path verified.
