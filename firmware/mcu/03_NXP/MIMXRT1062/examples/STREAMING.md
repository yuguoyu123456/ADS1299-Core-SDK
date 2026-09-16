# MIMXRT1062 250-SPS EEG and bounded streaming

Validation status: **TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED**.

After `probe_id.c`, `internal_test.c`, and `input_short.c` pass, use `eeg_250sps.c` for the first normal-electrode acquisition. It selects the shared-core `ADS1299_DR_250SPS` data-rate definition and normal-input MUX at gain 24 for each physically detected channel. The 250-SPS name is nominal for the ADS1299 default 2.048-MHz clock; if the board intentionally supplies another ADS1299 clock, derive the resulting rate from the TI clock/data-rate relationship rather than assuming 250 SPS.

`bounded_stream.c` demonstrates the next architectural step: acquisition produces decoded `ads1299_frame_t` objects into a fixed 16-slot ring and never performs UART/USB/network work in the DRDY path. When the consumer cannot keep up, the producer drops the newest frame and increments `ads1299_stream_dropped`; it never overwrites an unread frame silently. `ads1299_stream_produced` and each item sequence make loss observable.

The queue is static and bounded. A real MCUXpresso application should drain it from a foreground/RTOS transport task using `ads1299_stream_try_pop()`. If producer and consumer run in different interrupt/task contexts, replace the simple single-context index access with the synchronization primitive appropriate to that MCUXpresso/RTOS project; this template intentionally does not invent an RTOS or cache-coherency policy.

These examples do not define a wire protocol. If the application uses the repository canonical packet format, packetization and CRC belong in the transport/packet layer, not in the ADS1299 register driver or the DRDY acquisition path.

For another MIMXRT1062 board, keep these examples and the shared core unchanged; adapt the board/config layer for that board's IOMUXC, LPSPI instance/clock and GPIO assignments.
