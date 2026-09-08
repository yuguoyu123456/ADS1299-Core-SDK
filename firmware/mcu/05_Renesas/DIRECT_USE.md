# Renesas MCU Direct ADS1299 Use

This file adds a reusable ADS1299 integration path for the Renesas targets in this folder. Existing model-specific ports, examples, validation files and board support are left unchanged.

## Reuse the existing port abstraction

Each target already provides an `ads1299_port/` layer. Keep it and bind the generic callbacks to the platform SDK in use:

- RA2/RA4/RA6/RA8: Renesas FSP SPI/GPIO/IRQ resources.
- DA1469x: SmartBond SDK SPI/GPIO/interrupt resources.
- RX: FIT/Smart Configurator or register-level RSPI/GPIO.
- RL78: Smart Configurator / CSI or SPI-capable peripheral implementation.
- Synergy: SSP-generated SPI/GPIO resources.

The ADS1299 core only requires SPI transfer, CS/RESET/PWDN/START GPIO control, DRDY input and microsecond delay callbacks.

## ADS1299 bus setup

- SPI master.
- SPI Mode 1: CPOL=0, CPHA=1.
- MSB first.
- Start board bring-up conservatively around 2–4 MHz unless the selected low-power target requires a slower initial clock.
- Keep CS asserted for the complete ADS1299 command/register/frame transaction.

## DRDY acquisition flow

For ADS1299-8 continuous conversion, one frame is 27 bytes: 3 status bytes + 8 channels x 3 bytes.

Recommended pipeline:

`DRDY falling edge -> short ISR/callback -> high-priority acquisition context -> one full 27-byte frame -> ring buffer/queue -> USB/UART/Ethernet/BLE/storage/application`

Do not place logging, radio/network work, filesystem access or heavy signal processing in the DRDY callback.

## Shared ready-app helper

Add:

- `05_Renesas/_shared/renesas_ads1299_ready_app.c`
- `05_Renesas/_shared/renesas_ads1299_ready_app.h`
- the common `firmware/core_driver/ads1299` sources including `ads1299_profiles.c`

Available direct profiles:

- `RENESAS_ADS1299_PROFILE_EEG_250`
- `RENESAS_ADS1299_PROFILE_EEG_500`
- `RENESAS_ADS1299_PROFILE_EEG_1000`
- `RENESAS_ADS1299_PROFILE_INTERNAL_TEST`
- `RENESAS_ADS1299_PROFILE_INPUT_SHORT`

## Minimal application sequence

```c
ads1299_t ads;
renesas_ads1299_ready_state_t state = {0};
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

if (renesas_ads1299_prepare(&ads,
                            RENESAS_ADS1299_PROFILE_INTERNAL_TEST,
                            1) != ADS1299_OK) {
    /* handle error */
}

if (renesas_ads1299_start(&ads) != ADS1299_OK) {
    /* handle error */
}

/* Execute after a DRDY notification, outside the GPIO ISR. */
if (renesas_ads1299_read_one(&ads, &state) == ADS1299_OK) {
    /* enqueue state.last_frame */
}
```

## Validation order

1. Hardware reset.
2. Read and verify ADS1299 ID.
3. Internal test profile.
4. Input-short profile.
5. EEG 250 SPS profile.
6. Sustained continuous capture with frame/header checks.
7. Add BIAS and lead-off after the baseline stream is stable.
8. Increase rate only after the complete acquisition + transport path has been stress-tested.

## DMA/cache guidance

For higher-end RA devices, DMA/DTC can reduce CPU load. First establish a known-good polling/interrupt baseline, then add DMA with explicit buffer ownership. On cached devices, ensure DMA-accessible buffers and required cache maintenance are handled correctly.

## 8 x ADS1299 / 64 channels

For 64 channels, prefer multiple deterministic SPI/DMA lanes or an FPGA front-end when tight inter-device skew matters. Renesas MCUs can then handle configuration, monitoring, buffering and communication.

No fixed pin numbers are defined here because pinmux and board routing are board-specific.