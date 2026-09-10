# Validation

Current status: **Planned**

Only a directory scaffold and generic callback contract exist for STM32H533.
No target SDK adapter, clean target compile/link, hardware test or 24h test is claimed.
The example requires a real board binding. Generic test source existence is not a test pass.

Next evidence: exact device/board/SDK -> SPI/GPIO binding -> full target link ->
ADS1299 ID/internal-test capture -> sustained acquisition and loss accounting.

---

## Current maintenance validation state

The historical `Planned` statement above is retained as the original catalog record. Current maintenance state is **integration-candidate-complete** based on repository contents, with the following evidence boundaries:

| Evidence item | State |
|---|---|
| Reference board identified | PRESENT: NUCLEO-H533RE / STM32H533RET6 |
| Single board/config entry | PRESENT: `board/board_config.h` |
| Portable SPI/GPIO/DRDY port | PRESENT |
| Progressive probe/self-test/EEG example | PRESENT |
| Canonical packet integration in beginner example | PRESENT |
| Host integration test source/build recipe | PRESENT |
| Host test execution pass recorded | NO |
| STM32CubeH5 target compile/link | NOT BUILD-VERIFIED |
| Physical NUCLEO-H533RE + ADS1299 execution | NOT BOARD-VERIFIED |
| Sustained EXTI/DMA acquisition | NOT VERIFIED |
| Queue-overflow/loss measurement | NOT VERIFIED |
| Multi-ADS1299 hardware | NOT VERIFIED |
| 8xADS1299 / 64-channel operation | NOT VERIFIED |
| Long-run, EMC, electrical safety, production/regulatory readiness | NOT CLAIMED |

A source file or Makefile existing in the repository is not itself a test pass. Promote this model to **BUILD-VERIFIED** only after the documented STM32CubeH5 reference configuration actually compiles and links. Promote to **BOARD-VERIFIED** only after the stated board and ADS1299 hardware are physically run with recorded evidence.

### Next validation sequence

1. Run `tests/Makefile.host` on a host with the required compiler and record the exact pass/fail result.
2. Generate the NUCLEO-H533RE project with the documented STM32CubeH5 configuration and perform a clean compile/link.
3. On hardware, capture ADS1299 ID/probe and internal-test results, then input-short results.
4. Run finite 250-SPS acquisition and verify canonical packet reception/CRC handling.
5. Upgrade to EXTI/bounded buffering as needed and measure overflow/loss under sustained streaming.
6. Only after single-device stability is measured, evaluate shared-SPI/independent-CS multi-device operation and 64-channel scalability.
