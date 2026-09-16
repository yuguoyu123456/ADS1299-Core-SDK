# MIMXRT1062 progressive ADS1299 examples

Validation status: **TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED**.

Use these files as alternative application entry points in an official MCUXpresso MIMXRT1062 project. Do not compile more than one example `main()` at once. Hardware-dependent clocks, IOMUXC, LPSPI instance and GPIO pins remain isolated in `../board/evkb_reference.c`; ADS1299 register semantics remain in the shared core.

Recommended bring-up order:

1. `probe_id.c` — reset, leave RDATAC, read and validate the ADS1299-family ID. `ads1299_probe_result == 0` means the probe passed; 1..7 identifies the failed stage.
2. `internal_test.c` — enable the shared-core internal-test profile and inspect `ads1299_internal_test_frame` in the debugger.
3. `input_short.c` — enable the shared-core input-short profile and inspect `ads1299_input_short_frame`.
4. `main_ads1299.c` — existing continuous acquisition example. It keeps a debugger-visible latest frame and sequence guard.

The current leaf still needs a dedicated named 250-SPS EEG example and a bounded production streaming/queue example before it should be considered candidate-complete. These examples intentionally do not invent UART/USB transport policy or EVKB wiring beyond the existing board layer.
