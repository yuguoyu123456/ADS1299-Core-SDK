# Validation

Current status: **Planned**

Only a directory scaffold and generic callback contract exist for STM32F072.
No target SDK adapter, clean target compile/link, hardware test or 24h test is claimed.
The example requires a real board binding. Generic test source existence is not a test pass.

Next evidence: exact device/board/SDK -> SPI/GPIO binding -> full target link ->
ADS1299 ID/internal-test capture -> sustained acquisition and loss accounting.

---

## Current roll-up validation status

The legacy `Planned` paragraph above predates the current STM32F072 HAL binding, board configuration, runnable beginner example and host-test infrastructure. It is retained as history; the present status is:

**TEMPLATE / integration candidate-complete**

Evidence currently present in the repository:

- exact first reference board selected: NUCLEO-F072RB / STM32F072RBT6;
- one repository-owned board configuration entry point in `board/board_config.h`;
- STM32 HAL binding for SPI/GPIO/DRDY/delay/UART/timestamp in `examples/stm32f072_example_platform.c/.h`;
- shared-core based beginner sequence in `examples/stm32f072_beginner_demo.c/.h`;
- host adapter self-test under `ads1299_port/`;
- shared-core + STM32F072 port smoke-test runner under `tests/`;
- explicit CubeMX/CubeIDE integration recipe in `integration.md`.

Evidence **not** yet claimed:

- clean STM32CubeIDE target compile/link using a generated NUCLEO-F072RB project;
- flashing and execution on a physical NUCLEO-F072RB;
- ADS1299 ID/internal-test/input-short/EEG capture from physical hardware;
- sustained packet-loss, overrun, DMA or latency measurements;
- electrical safety, EMC, medical/regulatory or production validation.

### Evidence ladder for future status changes

1. **HOST-SOFTWARE-VERIFIED:** execute the host adapter and integration smoke tests with the documented compiler options and preserve the console result.
2. **BUILD-VERIFIED:** generate the stated STM32CubeIDE reference project, add the documented repository files, compile and link with zero unresolved mandatory callbacks.
3. **BOARD-VERIFIED:** flash the physical NUCLEO-F072RB and capture successful ID, internal-test, input-short and 250-SPS acquisition evidence from a real ADS1299-family device.
4. **SUSTAINED-ACQUISITION evidence:** run an explicitly timed acquisition while accounting for queue overflow, frame loss and transport backpressure. This is separate from BOARD-VERIFIED.

No later label should be inferred from source-file existence alone.
