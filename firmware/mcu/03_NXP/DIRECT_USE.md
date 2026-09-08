# NXP / MCUXpresso direct-use path

This vendor directory covers LPC, MCX, i.MX RT and Kinetis targets. Keep every model folder's existing board/port code; reuse `_shared/nxp_ads1299_ready_app.*` and the portable core-driver profile layer for the common acquisition sequence.

## MCUXpresso peripheral setup

1. Configure the SPI/FLEXCOMM/LPSPI instance connected to ADS1299 as master, SPI mode 1.
2. Start with a conservative 2-4 MHz SCLK during bring-up.
3. Configure CS/RESET/PWDN/START as GPIO outputs when routed to the MCU.
4. Configure DRDY as falling-edge GPIO interrupt.
5. In the GPIO ISR, only set a flag, give a semaphore, or notify the acquisition task.
6. Perform one exact 27-byte ADS1299-8 read per DRDY in the high-priority acquisition context.
7. Push completed frames into a ring buffer before USB/Ethernet/Wi-Fi/storage processing.

## Shared source files

Add the common core driver plus:

```text
firmware/mcu/03_NXP/_shared/nxp_ads1299_ready_app.c
firmware/mcu/03_NXP/_shared/nxp_ads1299_ready_app.h
```

The selected model's existing port layer should populate `ads1299_port_t` using MCUXpresso SPI/GPIO/delay functions.

## Minimal application sequence

```c
ads1299_t ads;
nxp_ads1299_stream_state_t stream;

ads1299_init(&ads, &port);
ads1299_device_id_t id;
ads1299_read_device_id(&ads, &id);

nxp_ads1299_prepare(&ads, NXP_ADS1299_PROFILE_INTERNAL_TEST);
nxp_ads1299_start(&ads, &stream);

/* Once for every DRDY notification */
nxp_ads1299_read_one(&ads, &stream);
```

Validation order is always:

```text
ID -> internal test -> input short -> normal EEG -> lead-off/BIAS -> transport stress
```

## DMA and cache

For i.MX RT/high-performance MCX devices, add LPSPI/Flexcomm DMA only after the blocking/interrupt path is stable. Keep DMA buffers in suitable memory and apply the platform's cache-coherency requirements when cache is enabled.

## 8 x ADS1299 / 64 channels

For low-skew 64-channel acquisition, multiple SPI/DMA lanes or FPGA parallel capture are preferred. NXP MCU can then perform control, frame aggregation, timestamping and transport.

Every NXP model directory receives a `READY_TO_RUN.md` entry pointing to this flow; no existing files are removed or replaced.
