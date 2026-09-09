# Validation

Current status: **Planned**

Only a directory scaffold and generic callback contract exist for STM32F103.
No target SDK adapter, clean target compile/link, hardware test or 24h test is claimed.
The example requires a real board binding. Generic test source existence is not a test pass.

Next evidence: exact device/board/SDK -> SPI/GPIO binding -> full target link ->
ADS1299 ID/internal-test capture -> sustained acquisition and loss accounting.

---

## Current validation roll-up

The original Planned note above is retained as historical context. The folder now contains a concrete NUCLEO-F103RB board/config layer, STM32F1 HAL binding, beginner demo and host-side test infrastructure, so the current status is:

**TEMPLATE / integration candidate-complete**

### Evidence ladder

1. **Repository integration present — YES**
   - `board/board_config.h`
   - STM32F103 SPI/GPIO/DRDY port adapter
   - STM32F1 HAL example binding
   - progressive beginner demo
   - host adapter and shared-core integration smoke-test infrastructure
2. **Host software tests — infrastructure present**
   - test runners and `Makefile.host` exist;
   - a passing result must only be recorded when a host compiler run is actually captured.
3. **BUILD-VERIFIED — NO**
   - no clean STM32CubeIDE compile/link result for the documented NUCLEO-F103RB reference project is recorded here.
4. **BOARD-VERIFIED — NO**
   - no physical NUCLEO-F103RB + ADS1299 probe/internal-test/input-short/EEG capture is recorded here.
5. **Sustained acquisition / loss accounting — NO**
   - no long-run buffer-overflow, packet-loss or timing evidence is recorded.
6. **Multi-ADS1299 / 64-channel validation — NO**
   - architecture guidance exists at repository level, but this model has no measured multi-device evidence.

### Promotion criteria

To claim **BUILD-VERIFIED**, record the exact STM32CubeIDE/STM32CubeF1 configuration and a clean compile/link of the shared core plus this model's port and example binding.

To claim **BOARD-VERIFIED**, additionally record physical-board evidence showing ADS1299 ID/probe success, internal-test behavior, input-short behavior, 250-SPS EEG acquisition and clean start/stop using the documented wiring.

Do not infer bench, electrical safety, EMC, production readiness or sustained-streaming status from source-code presence alone.
