# Espressif / ESP-IDF direct-use path

All ESP32-family folders keep their existing board/port/ESP-IDF content. New projects can share the acquisition application under `_shared/` and the common ADS1299 profile layer.

## ESP-IDF architecture

Recommended task split:

```text
GPIO ISR (DRDY falling edge)
    -> task notification / semaphore
        -> high-priority acquisition task
            -> 27-byte SPI transaction
                -> ring buffer / queue
                    -> Wi-Fi / BLE / USB / UART / storage task
```

Do not execute network transmission or a long SPI transaction inside the GPIO ISR.

## SPI setup

Use an SPI master device configured for ADS1299 mode 1. Start with a conservative 2-4 MHz SCLK during bring-up. The exact GPIO mapping is board-specific and must come from the target board schematic/project configuration.

## Shared sources

Add:

```text
firmware/core_driver/ads1299/ads1299.c
firmware/core_driver/ads1299/ads1299_frame.c
firmware/core_driver/ads1299/ads1299_multi.c
firmware/core_driver/ads1299/ads1299_model.c
firmware/core_driver/ads1299/ads1299_profiles.c
firmware/mcu/02_Espressif/_shared/espidf_ads1299_ready_app.c
```

Use the selected model folder's existing ESP-IDF port/adapter to create `ads1299_port_t`.

## Minimal sequence

```c
ads1299_t ads;
espidf_ads1299_stream_state_t stream;

ads1299_init(&ads, &port);
ads1299_device_id_t id;
ads1299_read_device_id(&ads, &id);

/* First validate the digital/analog signal path. */
espidf_ads1299_prepare(&ads, ESPIDF_ADS1299_PROFILE_INTERNAL_TEST);
espidf_ads1299_start(&ads, &stream);

/* Acquisition task, once per DRDY notification: */
espidf_ads1299_read_one(&ads, &stream);
```

Then test `INPUT_SHORT`; only after both are stable move to EEG profiles.

## FreeRTOS DRDY pattern

The GPIO ISR should use an ISR-safe task notification or semaphore. The acquisition task should have higher priority than Wi-Fi/BLE application tasks and should block waiting for DRDY notification.

A completed frame should be copied/pushed to a ring buffer immediately. Network tasks consume that queue asynchronously.

## Wireless streaming

Wi-Fi/BLE timing is not deterministic. Always attach a sample sequence number/timestamp at the packet layer and count queue overflows. Do not assume one wireless packet equals one ADS1299 sample.

## 64-channel architecture

An ESP32-family MCU is excellent for control and wireless transport, but an 8 x ADS1299 / 64-channel synchronous front end is easier to make deterministic with FPGA or a dedicated multi-SPI capture layer. A recommended architecture is:

```text
8 x ADS1299 -> FPGA/MCU acquisition -> framed 64-ch samples -> ESP32 -> Wi-Fi/BLE
```

Each Espressif model folder now contains `READY_TO_RUN.md` pointing to this flow.
