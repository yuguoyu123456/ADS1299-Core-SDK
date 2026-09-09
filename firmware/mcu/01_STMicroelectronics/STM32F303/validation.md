# Validation

Current status: **Planned**

Only a directory scaffold and generic callback contract exist for STM32F303.
No target SDK adapter, clean target compile/link, hardware test or 24h test is claimed.
The example requires a real board binding. Generic test source existence is not a test pass.

Next evidence: exact device/board/SDK -> SPI/GPIO binding -> full target link ->
ADS1299 ID/internal-test capture -> sustained acquisition and loss accounting.

---

## Current evidence roll-up

The initial scaffold note above is preserved as history. The repository has since gained a concrete NUCLEO-F303RE integration path.

| Evidence level | Current state |
|---|---|
| Reference board selected | YES — NUCLEO-F303RE / STM32F303RET6 |
| Repository board/config layer | PRESENT — `board/board_config.h` |
| STM32F3 HAL/platform binding | PRESENT — `examples/stm32f303_example_platform.c` plus `ads1299_port/` |
| Beginner probe/test/EEG/stream example | PRESENT — `examples/stm32f303_beginner_demo.c` |
| Port host self-test recipe | PRESENT — `ads1299_port/Makefile.host` |
| Shared-core/platform host integration test recipe | PRESENT — `tests/Makefile.host` |
| Host tests actually recorded PASS | NO CLAIM in this status file |
| STM32CubeIDE BUILD-VERIFIED | NO |
| NUCLEO-F303RE + ADS1299 BOARD-VERIFIED | NO |
| Sustained acquisition / overflow measurement | NO |
| Multi-ADS1299 / 64-channel validation | NO |

### Current classification

**TEMPLATE / repository integration candidate-complete.** This means the required board/config, port, beginner example and test infrastructure are present for the current repository-filling round. It does not imply target compilation or physical hardware success.

### Next validation evidence

1. Run the host port self-test and host integration smoke test and record exact compiler/version/output.
2. Import the documented sources into a NUCLEO-F303RE STM32CubeIDE project and achieve a clean compile/link without removing shared-core modules.
3. On physical hardware, record ADS1299 ID/probe and internal-test results.
4. Verify input-short and 250-SPS continuous acquisition with explicit dropped-frame/overflow accounting.
5. Only after single-device evidence, measure multi-ADS1299 shared-SPI/independent-CS timing and transport headroom.

No bench-tested, long-run-tested, electrical-safety, EMC, production-readiness or regulatory claim is made.