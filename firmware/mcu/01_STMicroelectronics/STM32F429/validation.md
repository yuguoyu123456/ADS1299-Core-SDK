# Validation

Current status: **TEMPLATE / integration candidate-complete**

The early catalog scaffold has now been extended with a concrete STM32F429 reference-board path, repository-owned board/config file, STM32F4 HAL/portable-port binding, progressive beginner example, and host integration smoke-test sources/build recipe. This status means the model folder is technically populated enough for the next validation stage; it does **not** imply successful target build or hardware execution.

## Evidence currently present

- Reference board identified: STM32F429I-DISC1 (MB1075), STM32F429ZIT6.
- `board/board_config.h`: present.
- STM32F429 `ads1299_port` implementation and host self-test source: present.
- Progressive beginner flow for probe/ID -> internal test -> input short -> 250-SPS EEG -> canonical stream -> clean stop: present.
- `tests/Makefile.host` integration smoke-test recipe: present.

## Evidence not yet recorded

| Validation step | Status |
|---|---|
| Host `make -f Makefile.host test` PASS | **NOT RECORDED** |
| Clean STM32CubeIDE build/link of progressive reference path | **NOT RECORDED** |
| Flash/run on STM32F429I-DISC1 | **NOT RECORDED** |
| ADS1299 ID/probe on physical board | **NOT RECORDED** |
| Internal-test/input-short physical capture | **NOT RECORDED** |
| 250-SPS sustained EEG acquisition with loss accounting | **NOT RECORDED** |
| Multi-ADS1299 / 64-channel acquisition | **NOT RECORDED** |
| 24-hour run, EMC, electrical safety, production or regulatory validation | **NOT CLAIMED** |

## Next validation evidence

1. Execute the host integration smoke test and record compiler/version plus PASS output.
2. Generate the documented STM32F429I-DISC1 Cube project and perform a clean build/link with the progressive example.
3. Flash the board and verify GPIO/SPI timing before attaching ADS1299 hardware.
4. Capture ID/probe, internal-test and input-short evidence on a physical ADS1299 setup.
5. Run 250-SPS streaming with explicit frame/queue overflow accounting.
6. Only after single-device stability, measure DMA/buffering/transport behavior for multiple ADS1299 devices.

Generic source existence is never counted as a test pass. Software build evidence and physical-board behavior remain separate validation levels.
