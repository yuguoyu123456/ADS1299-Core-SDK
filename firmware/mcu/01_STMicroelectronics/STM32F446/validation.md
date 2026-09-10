# Validation

Current round status: **integration candidate-complete**.

This label means the model folder now contains the board/config layer, STM32F446 portable HAL adapter, progressive beginner example and host integration smoke-test recipe. It does **not** imply that the target software has been build-verified or that ADS1299 hardware has been board-verified.

## Evidence matrix

| Evidence | Status |
|---|---|
| Board/config implementation present | YES |
| STM32F446 port/HAL implementation present | YES |
| Progressive beginner example present | YES |
| Host integration test recipe present | YES |
| Host test PASS recorded | NO |
| STM32CubeIDE clean build/link recorded | NO |
| NUCLEO-F446RE + ADS1299 BOARD-VERIFIED | NO |
| Internal-test capture from physical ADS1299 recorded | NO |
| Input-short capture from physical ADS1299 recorded | NO |
| 250-SPS sustained acquisition validated | NO |
| Overflow/loss accounting measured | NO |
| Multi-ADS1299 timing validated | NO |
| 8xADS1299 / 64-channel validation | NO |

## Validation sequence

The next evidence should be collected in this order:

1. run `tests/Makefile.host` and record compiler/version/result;
2. clean-build the progressive NUCLEO-F446RE project in STM32CubeIDE and record Cube/HAL/compiler versions;
3. run probe/ID on one ADS1299;
4. capture internal-test and input-short results;
5. verify the 250-SPS EEG stream and clean stop;
6. run sustained acquisition with explicit overflow/loss counters;
7. only then scale to multiple ADS1299 devices and measure timing/transport headroom.

Do not upgrade status to BUILD-VERIFIED without an actual successful documented build. Do not upgrade to BOARD-VERIFIED without evidence from the stated physical board and ADS1299 hardware.
