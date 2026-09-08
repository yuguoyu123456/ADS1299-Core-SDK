# Validation

Current status: **Planned**

Only a directory scaffold and generic callback contract exist for STM32F091.
No target SDK adapter, clean target compile/link, hardware test or 24h test is claimed.
The example requires a real board binding. Generic test source existence is not a test pass.

Next evidence: exact device/board/SDK -> SPI/GPIO binding -> full target link ->
ADS1299 ID/internal-test capture -> sustained acquisition and loss accounting.

---

## Current validation status (additive update)

The historical planning statement above is retained for traceability. The folder now contains a concrete NUCLEO-F091RC board/config layer, STM32 HAL binding, beginner demo, port self-test infrastructure and shared-core integration smoke-test infrastructure.

Current model status: **TEMPLATE / integration candidate-complete**.

### Evidence ladder

1. **Repository structure present — PASS**
   - board/config layer exists;
   - STM32F091 HAL/port layer exists;
   - beginner example covers ID/probe, internal test, input-short, 250-SPS EEG and canonical packet streaming;
   - host test entry points exist.
2. **Host adapter/integration execution — PARTIAL EVIDENCE**
   - test infrastructure is present in `ads1299_port/` and `tests/`;
   - prior maintenance runs exercised the adapter self-test against the repository interface, but this file does not promote the MCU target to BUILD-VERIFIED.
3. **STM32CubeIDE compile/link — NOT YET CLAIMED**
   - no clean reference-project build artifact or CI evidence is recorded here yet.
4. **Physical NUCLEO-F091RC + ADS1299 execution — NOT YET CLAIMED**
   - no physical ID capture, internal-test waveform capture or input-short result is recorded here yet.
5. **Sustained acquisition / loss accounting — NOT YET CLAIMED**
   - no long-run duration, packet-loss measurement, queue high-water mark, DMA timing evidence or multi-device timing evidence is recorded.

### Status promotion rules

Use **BUILD-VERIFIED** only after the documented STM32CubeIDE/reference configuration compiles and links successfully with the intended repository sources. This does not imply hardware communication.

Use **BOARD-VERIFIED** only after the physical reference board communicates with ADS1299 and evidence covers at least ID/probe plus the documented validation flows. Hardware validation must name the board, ADS1299 hardware and test configuration.

Do not infer bench-tested, long-run-tested, electrical safety, EMC, production readiness or regulatory status from either software build success or a short board bring-up.
