# Microchip MCU Direct ADS1299 Use

This file adds a reusable ADS1299 integration path for the Microchip targets in this folder. Existing model-specific ports, examples, tests and board support remain unchanged.

## Reuse the existing port layer

Each target already contains an `ads1299_port/` abstraction. Keep it and bind the generic callbacks to the SDK/peripheral layer used by the selected family:

- AVR128DA/DB and classic AVR: MPLAB XC8 / MCC SPI and GPIO or register-level drivers.
- PIC16/PIC18: MCC/Melody or register-level MSSP SPI and GPIO.
- PIC32 / PIC32CM / PIC32CZ / PIC32MZ: MPLAB Harmony 3 PLIB/driver SPI, GPIO and external interrupt resources.
- SAMC/SAMD/SAME/SAMG/SAML/SAMV: Harmony 3 PLIB/driver SERCOM/SPI, GPIO/EIC or device-specific SPI resources.

The common ADS1299 core only requires SPI transfer, CS/RESET/PWDN/START GPIO, DRDY input and microsecond delay callbacks.

## ADS1299 bus requirements

- SPI master.
- SPI Mode 1: CPOL=0, CPHA=1.
- MSB first.
- Start board bring-up conservatively around 2–4 MHz; for small 8-bit targets use a lower clock if the system clock requires it.
- Keep CS asserted for each complete ADS1299 command/register/frame transaction.

## DRDY acquisition pattern

ADS1299-8 continuous conversion produces one 27-byte frame per DRDY: 3 status bytes + 8 channels x 3 bytes.

Recommended flow:

`DRDY falling edge -> short ISR/callback -> high-priority acquisition context -> one full frame -> ring buffer/queue -> USB/UART/Ethernet/CAN/storage/application`

Do not perform logging, network work, filesystem access or heavy signal processing inside the DRDY ISR.

## Shared ready-app helper

Add:

- `06_Microchip/_shared/microchip_ads1299_ready_app.c`
- `06_Microchip/_shared/microchip_ads1299_ready_app.h`
- the common `firmware/core_driver/ads1299` sources including `ads1299_profiles.c`

Available direct profiles:

- `MICROCHIP_ADS1299_PROFILE_EEG_250`
- `MICROCHIP_ADS1299_PROFILE_EEG_500`
- `MICROCHIP_ADS1299_PROFILE_EEG_1000`
- `MICROCHIP_ADS1299_PROFILE_INTERNAL_TEST`
- `MICROCHIP_ADS1299_PROFILE_INPUT_SHORT`

## Minimal application sequence

```c
ads1299_t ads;
microchip_ads1299_ready_state_t state = {0};
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

if (microchip_ads1299_prepare(&ads,
                              MICROCHIP_ADS1299_PROFILE_INTERNAL_TEST,
                              1) != ADS1299_OK) {
    /* handle error */
}

if (microchip_ads1299_start(&ads) != ADS1299_OK) {
    /* handle error */
}

/* Run after DRDY notification, outside the GPIO ISR. */
if (microchip_ads1299_read_one(&ads, &state) == ADS1299_OK) {
    /* enqueue state.last_frame */
}
```

## Validation sequence

1. Hardware reset.
2. Read and verify ADS1299 ID.
3. Internal test profile.
4. Input-short profile.
5. EEG 250 SPS profile.
6. Sustained capture with frame/header checks.
7. Add BIAS and lead-off only after stable baseline capture.
8. Increase the sample rate after the complete acquisition and transport path has been stress-tested.

## Small-MCU guidance

For AVR/PIC16/PIC18-class devices, keep raw samples in integer form, use static buffers, avoid floating-point conversion in the timing-critical path and verify the final UART/USB link budget before enabling faster data rates.

## DMA/cache guidance

For higher-end PIC32/SAM devices, first establish a known-good polling/interrupt baseline, then add DMA with explicit buffer ownership. If the target has data cache, ensure DMA-accessible memory and required cache maintenance are handled correctly.

## 8 x ADS1299 / 64 channels

For 64 channels, use multiple deterministic SPI/DMA lanes or an FPGA front-end when tight cross-device timing matters. Microchip MCUs can then handle control, buffering, timestamping, DSP and transport according to their performance class.

No fixed pin numbers are specified because board routing and pinmux are target-specific.