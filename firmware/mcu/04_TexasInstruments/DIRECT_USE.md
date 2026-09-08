# Texas Instruments MCU Direct ADS1299 Use

This file adds a common direct-use path for the TI MCU families in this folder. Existing per-device ports, examples, tests and documentation remain unchanged.

## Reuse the existing port layer

Each supported TI target already contains an `ads1299_port/` abstraction. Keep that code and bind its generic callbacks to the SDK used by the selected device family:

- SimpleLink / TI Drivers for CC13xx, CC26xx, CC23xx, CC27xx and CC32xx families.
- MSPM0 DriverLib / SysConfig-generated peripherals for MSPM0 devices.
- TivaWare or TI DriverLib for TM4C devices.
- MSP430 DriverLib or register-level SPI/GPIO for MSP430 targets.
- C2000 DriverLib for TMS320F28379D.

The common ADS1299 core only needs SPI transfer, CS/RESET/PWDN/START GPIO, DRDY input and microsecond delay callbacks.

## ADS1299 SPI requirements

- Master mode.
- SPI Mode 1: CPOL=0, CPHA=1.
- MSB first.
- Start bring-up conservatively around 2–4 MHz unless the board-specific timing has already been validated.
- Do not toggle CS in the middle of a command/register/frame transaction.

## DRDY acquisition pattern

For ADS1299-8 in continuous mode, one conversion frame is 27 bytes: 3 status bytes + 8 channels x 3 bytes.

Recommended architecture:

`DRDY falling edge -> short ISR/callback -> high-priority acquisition context -> one complete ADS1299 frame -> ring buffer/queue -> USB/UART/BLE/Wi-Fi/Ethernet/storage task`

The interrupt/callback should not perform radio, filesystem, printf, signal processing or long blocking operations.

## Shared helper

Add these sources to the target project:

- `../core_driver/ads1299/ads1299.c`
- `../core_driver/ads1299/ads1299_frame.c`
- `../core_driver/ads1299/ads1299_model.c`
- `../core_driver/ads1299/ads1299_multi.c`
- `../core_driver/ads1299/ads1299_profiles.c`
- `04_TexasInstruments/_shared/ti_ads1299_ready_app.c`

and include:

- `ads1299.h`
- `ads1299_profiles.h`
- `ti_ads1299_ready_app.h`

The helper exposes five direct profiles:

- `TI_ADS1299_PROFILE_EEG_250`
- `TI_ADS1299_PROFILE_EEG_500`
- `TI_ADS1299_PROFILE_EEG_1000`
- `TI_ADS1299_PROFILE_INTERNAL_TEST`
- `TI_ADS1299_PROFILE_INPUT_SHORT`

## Minimal application flow

```c
ads1299_t ads;
ti_ads1299_ready_state_t state = {0};
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

if (ti_ads1299_prepare(&ads, TI_ADS1299_PROFILE_INTERNAL_TEST, 1) != ADS1299_OK) {
    /* handle error */
}

if (ti_ads1299_start(&ads) != ADS1299_OK) {
    /* handle error */
}

/* Call after each DRDY notification, outside the GPIO ISR. */
if (ti_ads1299_read_one(&ads, &state) == ADS1299_OK) {
    /* enqueue state.last_frame */
}
```

## Recommended validation sequence

1. Hardware reset.
2. Read and verify device ID.
3. Internal test profile.
4. Input-short profile.
5. EEG 250 SPS profile.
6. Confirm long continuous capture with no frame misalignment.
7. Add BIAS and lead-off features.
8. Increase sample rate only after the baseline path is stable.
9. Stress the final communication link and monitor queue overflow/read errors.

## TI wireless-family note

For CC13xx/CC26xx/CC23xx/CC27xx/CC32xx targets, keep radio stacks separated from the acquisition callback. Use a queue between ADS1299 capture and BLE/Wi-Fi/Sub-1 GHz transport so RF scheduling never blocks the DRDY service path.

## 8 x ADS1299 / 64 channels

For a 64-channel system, do not assume one serialized SPI transaction path is automatically adequate. If tight inter-device timing is important, use an FPGA capture stage or several independent SPI/DMA engines. TI MCUs can then handle configuration, health monitoring, packetization and communication.

No fixed pin numbers are defined here because pinmux and board routing are target-board specific.