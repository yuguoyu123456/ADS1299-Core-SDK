# Validation

Current round status: **INTEGRATION CANDIDATE-COMPLETE**

This status means the STM32F767 model folder now contains enough repository-owned integration structure to be a candidate for target build and beginner acceptance testing. It does **not** mean BUILD-VERIFIED or BOARD-VERIFIED.

## Evidence present in the repository

- Reference board is explicit: NUCLEO-F767ZI (MB1137, STM32F767ZIT6).
- `board/board_config.h` provides one obvious board/config entry point.
- STM32F767 SPI/GPIO/DRDY adapter sources are present under `ads1299_port/`.
- The progressive example performs probe/ID, internal test, input-short, 250-SPS gain-24 EEG setup, DRDY-paced reads, canonical packet streaming and clean stop.
- The original `examples/main_ads1299.c` remains present as a compatibility path.
- `tests/Makefile.host` provides a model-local port + shared-core integration smoke-test recipe.
- Diagnostics distinguish reset/power, SPI/ID, DRDY timeout, frame-read and stream-transport failures.

## Evidence NOT yet recorded

| Validation item | Status |
|---|---|
| Host integration smoke-test execution | NOT RECORDED |
| STM32CubeIDE clean compile/link for the documented reference configuration | NOT RECORDED |
| NUCLEO-F767ZI + ADS1299 physical execution | NOT RECORDED |
| Probe/ID capture from real hardware | NOT RECORDED |
| Internal-test/input-short capture from real hardware | NOT RECORDED |
| Sustained 250-SPS packet-loss/overflow measurement | NOT RECORDED |
| DMA/cache-coherency implementation validation | NOT RECORDED |
| Multi-ADS1299 timing/buffering validation | NOT RECORDED |
| 8x ADS1299 / 64-channel validation | NOT RECORDED |
| 24-hour/long-run testing | NOT RECORDED |

## Next validation sequence

1. Run the host integration smoke test and record the compiler/version plus PASS result.
2. Generate the documented NUCLEO-F767ZI Cube project and perform a clean STM32CubeIDE build/link.
3. Flash and capture the ID/probe and both beginner self-test stages on real ADS1299 hardware.
4. Run the finite 1000-packet 250-SPS path and measure transport/overflow behavior.
5. Only after the blocking starter is stable, implement/validate IRQ/DMA + bounded buffering and STM32F7 cache handling.
6. Then expand to multi-device timing and 64-channel scalability tests.

Do not upgrade this folder to BUILD-VERIFIED or BOARD-VERIFIED without actual recorded evidence. Source presence alone is not a test pass.
