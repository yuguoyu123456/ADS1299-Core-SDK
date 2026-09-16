# LPC54628 250-SPS EEG and bounded streaming

Validation status: **TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED**.

After `probe_id.c`, `internal_test.c`, and `input_short.c` pass on your hardware, use `eeg_250sps.c` for the first normal-electrode acquisition. It uses the shared typed ADS1299 API to select 250 SPS, gain 24, and normal electrode input for every channel reported by the probed device. No ADS1299 register literals belong in the LPC54628 board layer.

Use `bounded_stream.c` when transport work must be separated from acquisition. The producer reads one ADS1299 frame when DRDY is active and pushes it into a fixed 16-slot ring. A full ring drops the newest frame and increments `ads1299_stream_dropped`, making overload visible instead of silently overwriting unread data. UART, USB, Ethernet, logging, or packet encoding should consume with `ads1299_stream_try_pop()` outside the timing-sensitive acquisition path.

The example intentionally does not pretend to provide ISR/RTOS synchronization. If producer and consumer run in different interrupt/task contexts, protect the queue indices with the synchronization mechanism appropriate to the actual MCUXpresso/RTOS project. Likewise, DMA/cache handling must follow the concrete LPC54628 SDK project rather than being invented in this template.

These files do not change the board contract: hardware-dependent SPI/GPIO/pin/clock choices remain in the LPC54628 board/config/port integration. They do not require editing the shared ADS1299 core.
