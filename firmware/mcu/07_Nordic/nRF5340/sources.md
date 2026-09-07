# Source and license record

The adapter and overlay are original project code using the following official
interfaces and board facts, not copies of a third-party ADS1299 driver:

- [Zephyr v3.7.1](https://github.com/zephyrproject-rtos/zephyr/tree/v3.7.1),
  Apache-2.0 project (individual file notices retained externally).
- [Nordic-authored nRF5340 DK board map](https://github.com/zephyrproject-rtos/zephyr/blob/v3.7.1/boards/nordic/nrf5340dk/nrf5340_cpuapp_common.dtsi),
  confirming Arduino D2-D9 mapping to P1.04-P1.11 and SPIM4 availability.
- [Nordic HAL revision](https://github.com/zephyrproject-rtos/hal_nordic/tree/ab5cb2e2faeb1edfad7a25286dcb513929ae55da),
  using its unmodified nrfx/MDK with original Nordic notices externally.
- [CMSIS revision](https://github.com/zephyrproject-rtos/cmsis/tree/4b96cbb174678dcd3ca86e11e1f24bc5f8726da0),
  with the upstream license retained in the external installation.

Check build.md for exact verified versions. NCS 3.4.0 tag availability was
checked, but its CI workflow was not executed. No hardware compatibility or
BLE capability has been inferred from merely having a build definition.
