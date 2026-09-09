# Validation

Current status: **BUILD-VERIFIED for the recorded PlatformIO reference build; BOARD-VERIFIED is not claimed.**

Legacy lifecycle labels retained by this folder are `Planned`, `Reference`,
`Example`, `Compatible`, `Compiles`, `Bench-tested`, `24h-tested`. For current
SDK maintenance, interpret the evidence explicitly rather than relying on one
ambiguous label.

## Recorded software build evidence

Build entrypoint: `platformio run -d firmware/mcu/01_STMicroelectronics/STM32F407`.
A clean local build passed on **2026-09-04**.

The clean build used:

- PlatformIO Core 6.1.19
- ST STM32 platform 20.0.0
- STM32CubeF4 framework 1.28.3
- GNU Arm Embedded 7.2.1 supplied by the installed platform
- PlatformIO board definition `disco_f407vg`

The successful link used 10,004 bytes of Flash and 132 bytes of static RAM.
See `build.md` for the exact command and vendor-warning notes. This is valid
software BUILD-VERIFIED evidence only for the documented reference build.

## Current evidence ladder

| Level | Status | Evidence required / present |
|---|---|---|
| Repository integration present | YES | board/config, port, examples and tests are present |
| Host/integration test infrastructure | YES | `ads1299_port/Makefile.host` and `tests/Makefile.host` exist |
| Recorded host test PASS | NOT YET RECORDED | retain logs before upgrading |
| Reference software BUILD-VERIFIED | YES | clean PlatformIO build dated 2026-09-04 |
| Progressive beginner-demo CubeIDE build | NOT YET RECORDED | clean CubeIDE build/log required |
| BOARD-VERIFIED with ADS1299 | NO | real board, ID, self-test and acquisition evidence required |
| Sustained acquisition verified | NO | bounded-buffer/overflow/loss/CRC evidence required |
| Multi-ADS1299 / 64-channel verified | NO | independent-CS topology, timing and throughput evidence required |

To advance hardware status, attach the exact board revision, wiring/config,
ADS1299 ID read, internal-test and input-short results, 250-SPS EEG acquisition,
packet-integrity evidence and the firmware commit used. Sustained validation must
also include explicit loss/overflow/error counters.

Do not use BUILD-VERIFIED as evidence of electrical safety, EMC, production
readiness, analog-front-end correctness or physical ADS1299 behavior.
