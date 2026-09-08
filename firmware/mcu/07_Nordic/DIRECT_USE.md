# Nordic MCU Direct ADS1299 Use

This file adds a reusable ADS1299 integration path for the Nordic targets in this folder. Existing model-specific ports, examples, tests and board support remain unchanged.

## Reuse the existing port layer

Each Nordic target already contains an `ads1299_port/` abstraction. Keep it and bind the generic callbacks to the SDK used by the target project:

- nRF52832/nRF52833/nRF52840: nRF Connect SDK/Zephyr `spi_dt_spec` + GPIO callbacks, or legacy nRF5 SDK SPIM/GPIOTE where the existing project still uses it.
- nRF5340/nRF54: nRF Connect SDK/Zephyr, with SPIM and GPIO/interrupt resources selected through devicetree.
- nRF9151: nRF Connect SDK/Zephyr, keeping cellular/network work separated from acquisition.

The ADS1299 core needs only SPI transfer, CS/RESET/PWDN/START GPIO, DRDY input and microsecond delay callbacks.

## ADS1299 SPI setup

- SPI master.
- SPI Mode 1: CPOL=0, CPHA=1.
- MSB first.
- Start board bring-up around 2–4 MHz.
- Keep chip select valid for each complete ADS1299 command/register/frame transaction.

## DRDY acquisition pattern

For ADS1299-8 continuous mode, one DRDY corresponds to one 27-byte frame: 3 status bytes + 8 channels x 3 bytes.

Recommended architecture:

`DRDY falling edge -> short GPIO/GPIOTE callback -> semaphore/event/task notification -> high-priority acquisition context -> one full 27-byte SPIM transfer -> ring buffer -> BLE/USB/cellular/application`

Do not perform BLE notifications, cellular socket work, logging, filesystem access or heavy signal processing inside the DRDY callback.

## Shared ready-app helper

Add:

- `07_Nordic/_shared/nordic_ads1299_ready_app.c`
- `07_Nordic/_shared/nordic_ads1299_ready_app.h`
- the common `firmware/core_driver/ads1299` sources including `ads1299_profiles.c`

Available direct profiles:

- `NORDIC_ADS1299_PROFILE_EEG_250`
- `NORDIC_ADS1299_PROFILE_EEG_500`
- `NORDIC_ADS1299_PROFILE_EEG_1000`
- `NORDIC_ADS1299_PROFILE_INTERNAL_TEST`
- `NORDIC_ADS1299_PROFILE_INPUT_SHORT`

## Minimal application sequence

```c
ads1299_t ads;
nordic_ads1299_ready_state_t state = {0};
ads1299_port_t port = ads1299_mcu_make_port(&platform_port);

if (ads1299_init(&ads, &port) != ADS1299_OK) {
    /* handle error */
}
if (ads1299_hardware_reset(&ads) != ADS1299_OK) {
    /* handle error */
}

ads1299_device_id_t id;
if (ads1299_read_device_id(&ads, &id) != ADS1299_OK) {
    /* handle error */
}

if (nordic_ads1299_prepare(&ads,
                           NORDIC_ADS1299_PROFILE_INTERNAL_TEST,
                           1) != ADS1299_OK) {
    /* handle error */
}
if (nordic_ads1299_start(&ads) != ADS1299_OK) {
    /* handle error */
}

/* Execute after DRDY notification, outside the GPIO callback. */
if (nordic_ads1299_read_one(&ads, &state) == ADS1299_OK) {
    /* enqueue state.last_frame */
}
```

## EasyDMA / GPIOTE / PPI-DPPI guidance

Nordic SPIM EasyDMA can reduce CPU occupancy. First establish a known-good synchronous transfer path. Then use EasyDMA with static RAM buffers and explicit ownership between the transfer completion event and the consumer.

Where a device provides GPIOTE plus PPI/DPPI, these peripherals can reduce trigger latency and jitter, but hardware-triggered capture should be introduced only after the simpler software-triggered path is validated. Do not assume a specific channel/task/event mapping across all Nordic families.

## Wireless scheduling

BLE or cellular scheduling must be treated as a downstream transport constraint. Use a bounded ring buffer between ADS1299 acquisition and wireless packetization. If the link cannot keep up, increment an overflow/drop counter rather than corrupting sample timing.

## Validation sequence

1. Hardware reset ADS1299.
2. Read and verify ID.
3. Internal test profile.
4. Input-short profile.
5. EEG 250 SPS profile.
6. Long capture with radio/network idle.
7. Repeat with heavy BLE/cellular traffic and check frame alignment, read errors and queue overflow.
8. Add BIAS/lead-off and higher sample rates only after the baseline path is stable.

## 8 x ADS1299 / 64 channels

For 64 channels, use multiple deterministic SPI/DMA lanes or an FPGA capture front-end when tight inter-device skew matters. Nordic MCUs are especially useful for control and wireless transport after deterministic acquisition has been established.

No fixed pin numbers are defined because Nordic board pinmux/devicetree assignments are board-specific.