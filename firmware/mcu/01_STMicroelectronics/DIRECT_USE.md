# STMicroelectronics direct-use path

This vendor directory contains many STM32/STM8 model folders. Keep the existing per-model `ads1299_port`, `board`, `examples`, `integration` and `stm32cube_adapter` content; new applications should reuse the shared acquisition application in `_shared/` instead of duplicating profile and stream logic.

## Recommended STM32CubeMX configuration

For STM32 families with Cube HAL:

1. Enable one SPI peripheral as master, full duplex.
2. Use ADS1299-compatible SPI mode 1: clock idle low, capture on the second edge.
3. Start with 2-4 MHz SCLK for board bring-up; increase only after the link is proven stable and timing is checked.
4. Configure `CS`, `RESET`, `PWDN`, `START` as GPIO outputs when those signals are routed to the MCU.
5. Configure `DRDY` as falling-edge EXTI input.
6. Do not perform the 27-byte SPI transaction inside a long EXTI ISR. The ISR should only set a flag or release a task/semaphore; perform the SPI read in the high-priority acquisition context.
7. Keep one monotonically increasing sample counter and one transport/recording queue.

## Shared source files

Add these repository sources to the application build:

```text
firmware/core_driver/ads1299/ads1299.c
firmware/core_driver/ads1299/ads1299_frame.c
firmware/core_driver/ads1299/ads1299_multi.c
firmware/core_driver/ads1299/ads1299_model.c
firmware/core_driver/ads1299/ads1299_profiles.c
firmware/mcu/01_STMicroelectronics/_shared/stm32_ads1299_ready_app.c
```

Include directories:

```text
firmware/core_driver/ads1299
firmware/mcu/01_STMicroelectronics/_shared
```

Then use the existing model-specific port/Cube adapter to populate `ads1299_port_t` and call `ads1299_init()`.

## Minimal application flow

```c
ads1299_t ads;
stm32_ads1299_stream_state_t stream;

/* 1. Build ads1299_port_t from the selected model's existing adapter. */
ads1299_init(&ads, &port);

/* 2. Verify the real part before configuring it. */
ads1299_device_id_t id;
ads1299_read_device_id(&ads, &id);

/* 3. First hardware validation: internal square wave. */
stm32_ads1299_prepare(&ads, STM32_ADS1299_PROFILE_INTERNAL_TEST);
stm32_ads1299_start(&ads, &stream);

/* 4. In the DRDY-driven acquisition context: */
stm32_ads1299_read_one(&ads, &stream);
```

After the internal test signal is stable, repeat with `STM32_ADS1299_PROFILE_INPUT_SHORT`; only then move to `STM32_ADS1299_PROFILE_EEG_250` or a board-specific profile.

## DRDY event pattern

Recommended bare-metal pattern:

```c
static volatile uint8_t ads1299_drdy_pending;

void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if (pin == ADS1299_DRDY_Pin)
        ads1299_drdy_pending = 1u;
}

void acquisition_poll(void)
{
    if (!ads1299_drdy_pending)
        return;

    ads1299_drdy_pending = 0u;
    if (stm32_ads1299_read_one(&ads, &stream) == ADS1299_OK) {
        /* push stream.last_frame to a ring buffer / USB / UART / Ethernet */
    }
}
```

For FreeRTOS, use an ISR-safe semaphore/task notification instead of a polling flag.

## 8 x ADS1299 / 64-channel recommendation

For your 64-channel architecture, a high-performance STM32 can either:

- drive eight independent CS lines on one SPI bus and service each device sequentially after a shared DRDY event;
- use multiple SPI peripherals/DMA streams where the MCU supports it;
- or hand deterministic parallel capture to FPGA and let STM32 handle configuration/control/transport.

The third architecture is the most scalable when all eight ADS1299 devices must be sampled with minimal inter-device skew.

## Validation order

```text
power/reset
  -> ID register
  -> internal test square wave
  -> input short noise
  -> normal input
  -> lead-off/BIAS checks
  -> transport stress test
  -> multi-device synchronization test
```

Every model folder under this vendor now has a `READY_TO_RUN.md` entry that points to the exact same shared workflow without deleting or replacing the existing model-specific files.
