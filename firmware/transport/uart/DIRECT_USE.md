# UART transport direct-use guide

This directory is additive. Existing files are retained unchanged.

## Wire format

The canonical ADS1299 packet is 49 bytes and may contain zero bytes. `ads1299_uart_frame.[ch]` therefore wraps each validated packet with COBS and appends one `0x00` delimiter.

Benefits:

- no `0x00` appears inside an encoded frame;
- one delimiter always marks the end of a frame;
- after dropped/corrupted bytes the receiver can resynchronize at the next delimiter;
- the canonical ADS1299 CRC still performs end-to-end payload validation.

Maximum wire size for one 49-byte packet is 51 bytes including the delimiter.

## Transmit path

1. Build one canonical 49-byte packet.
2. Call `ads1299_uart_encode_packet()` into a buffer of at least `ADS1299_UART_MAX_WIRE_SIZE`.
3. Queue the encoded bytes to UART DMA or a non-blocking UART task.
4. Never perform a blocking 51-byte UART write inside the ADS1299 DRDY ISR.
5. Count queue overflow and UART driver errors.

Recommended architecture:

`DRDY -> SPI/DMA capture -> packet queue -> UART COBS encode -> UART DMA`

## Receive path

1. Initialize `ads1299_uart_decoder_t`.
2. Feed every received byte to `ads1299_uart_decoder_push()`.
3. Return `1` means one complete validated 49-byte packet is ready.
4. Return `0` means more bytes are needed.
5. Negative return means the current frame was malformed/overflowed and discarded.
6. Track `frames_ok`, `frames_bad` and `overflows` for diagnostics.

The decoder deliberately waits for the next zero delimiter after an oversized frame so random data cannot immediately be mistaken for a new packet.

## Baud-rate sizing

UART 8N1 normally consumes 10 line bits per transmitted byte.

One ADS1299 at 250 SPS:

- worst-case framed size: about 51 bytes/sample;
- line rate: `51 * 250 * 10 = 127,500 bit/s`;
- therefore 115200 baud is not sufficient for continuous worst-case streaming;
- use at least 230400 baud for a single-device 250-SPS baseline, with additional margin preferred.

Eight ADS1299 devices at 250 SPS:

- about `51 * 250 * 8 * 10 = 1,020,000 bit/s` before any application overhead;
- use a stable high-speed UART such as 1.5/2/3 Mbaud only if both endpoints and wiring support it;
- for a production 64-channel system, USB or Ethernet is usually a better primary transport.

Higher sample rates scale linearly.

## DMA and buffering

For MCUs with DMA:

- use a small pool/ring of encoded TX buffers;
- let DMA completion recycle buffers;
- do not reuse a buffer while DMA still owns it;
- for cached MCUs, apply the platform's DMA/cache coherency rules;
- keep transport queue capacity bounded so overload is observable rather than hidden.

For small MCUs:

- avoid dynamic allocation;
- keep raw ADS1299 values as integers;
- use fixed 51-byte buffers;
- consider lower SPS or fewer active channels if memory/baud limits are tight.

## Robustness rules

Do not use printable ASCII/CSV as the primary embedded streaming format. CSV is useful for diagnostics but wastes bandwidth and CPU.

Do not rely only on the packet sync bytes `0xA5 0x5A` for UART framing. COBS delimiter recovery plus the existing CRC gives stronger resynchronization after serial corruption.

## Validation

1. Run `uart_frame_smoke_test.c` with the common packet implementation.
2. Confirm packets containing many zero bytes encode/decode correctly.
3. Corrupt one encoded byte and confirm the frame is rejected.
4. Drop bytes and confirm the next delimiter restores parser synchronization.
5. Stress UART DMA while ADS1299 internal test signal runs.
6. Log packet sequence gaps, `frames_bad`, decoder overflows and TX queue overflows.
7. Test the exact cable length, voltage levels and baud rate intended for the product.

## Board-specific items intentionally not fixed here

The SDK does not hard-code UART instance, TX/RX pins, baud rate, flow-control pins or DMA channels because those depend on the selected board.
