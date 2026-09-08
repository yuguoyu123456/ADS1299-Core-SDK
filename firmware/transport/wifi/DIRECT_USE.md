# Wi-Fi transport direct-use guide

This directory is additive. Existing files are retained unchanged.

## Design goal

Wi-Fi must never control ADS1299 acquisition timing.

Radio scheduling, association, roaming, DHCP, TCP retransmission and socket back-pressure can all pause for much longer than one ADS1299 sample period. The acquisition path therefore needs a bounded queue between packet production and the Wi-Fi stack.

`ads1299_wifi_queue.[ch]` provides a fixed-capacity queue for already-validated canonical 49-byte ADS1299 packets. It uses no dynamic allocation.

## Recommended architecture

`DRDY -> SPI/DMA capture -> canonical packet -> bounded Wi-Fi queue -> network task -> UDP/TCP socket`

The DRDY ISR should only signal/timestamp acquisition work. Do not call Wi-Fi, DNS, DHCP or socket APIs from the DRDY ISR.

## Queue configuration

Default depth is 32 canonical packets:

`ADS1299_WIFI_QUEUE_DEPTH = 32`

Override the macro at build time or before including the header when a different fixed depth is required.

Memory cost is approximately:

`queue_depth * 49 bytes + small bookkeeping`

Choose the depth from measured worst-case Wi-Fi scheduling jitter, not from guesswork.

## Overload policies

### `ADS1299_WIFI_DROP_NEWEST`

When the queue is full, reject the newly produced packet and preserve all older queued data.

Useful when:

- preserving FIFO history is more important than freshness;
- another layer records/retries older data;
- the application wants an explicit failure instead of silently replacing history.

The queue increments `dropped_newest`.

### `ADS1299_WIFI_DROP_OLDEST`

When the queue is full, discard the oldest queued packet and accept the newest packet.

Useful for live EEG visualization/control where recent samples are more valuable than stale queued samples.

The queue increments `dropped_oldest` and returns `1` to make the loss visible.

If loss is unacceptable during long Wi-Fi outages, RAM buffering alone is not enough. Add local nonvolatile storage or another reliable transport rather than growing the RAM queue without bound.

## UDP framing

For low-latency streaming, reuse the completed Ethernet block format in:

`../ethernet/ads1299_ethernet_frame.[ch]`

Recommended flow:

1. pop 1–8 canonical packets from the Wi-Fi queue;
2. encode them into one Ethernet/network block;
3. send exactly one block per UDP datagram;
4. use block sequence plus canonical packet sequence numbers to detect loss.

For an 8×ADS1299 64-channel sample point, eight canonical packets plus the network block header remain well below a normal 1500-byte MTU, so application-level IP fragmentation is unnecessary.

## TCP framing

For reliable ordered delivery, reuse the same Ethernet/network block format over a TCP socket.

Remember that TCP is a byte stream:

- one `send()` does not imply one `recv()`;
- the receiver must first collect the fixed network header;
- call `ads1299_ethernet_frame_peek_size()`;
- then collect the remaining bytes before full decode.

Do not pop a packet permanently from the queue until your network task has decided how it handles partial/failed sends. A common pattern is to `peek`, attempt/complete the send, then `pop` only after ownership has safely transferred to the socket/TCP buffering layer.

## Reconnect behavior

When Wi-Fi disconnects:

1. acquisition continues;
2. stop socket sends;
3. keep queueing only up to the configured bound;
4. count dropped packets according to the selected policy;
5. reconnect Wi-Fi/socket in a lower-priority network task;
6. resume sending without resetting ADS1299 sample sequence numbers.

Never reset sequence counters simply because the radio reconnects. The host should be able to see exactly where samples were lost.

Track at least:

- Wi-Fi disconnect/reconnect count;
- socket send failures;
- queue `high_watermark`;
- `dropped_newest`;
- `dropped_oldest`;
- canonical packet sequence gaps;
- network block sequence gaps.

## Thread/ISR safety

`ads1299_wifi_queue_t` is intentionally a small portable data structure and does not contain an RTOS mutex/critical-section implementation.

If one execution context pushes while another pops, protect queue operations using the platform's appropriate primitive:

- short RTOS critical section;
- mutex when both sides are task context;
- lock-free/single-producer-single-consumer wrapper if formally implemented for the platform.

Do not assume `volatile` alone makes the queue safe across ISR/task or multiple cores.

## 64-channel throughput

At 250 SPS:

- one ADS1299 canonical stream: about 12.25 kB/s;
- eight ADS1299 devices: about 98 kB/s before network headers.

Normal Wi-Fi bandwidth is more than sufficient, but latency/jitter and coexistence with BLE or other radios can still create short bursts of back-pressure. The bounded queue is therefore required even when average bandwidth looks generous.

At higher SPS, scale the data rate linearly and measure queue high-water mark under real RF conditions.

## Wireless coexistence

On combo-radio MCUs:

- keep BLE/Wi-Fi callbacks out of the DRDY path;
- give SPI/DMA acquisition deterministic priority;
- measure worst-case latency while Wi-Fi is actively transmitting, scanning and reconnecting;
- avoid long critical sections in radio callbacks;
- use a dedicated acquisition task/core when the platform supports it and measurement justifies it.

## Validation

1. Run `wifi_queue_smoke_test.c` with the common packet implementation.
2. Verify FIFO push/pop with valid canonical packets.
3. Fill the queue and verify both overload policies and counters.
4. Corrupt a packet CRC and confirm it is rejected before entering the queue.
5. On hardware, run ADS1299 internal test signal while repeatedly associating/disassociating Wi-Fi.
6. Artificially block socket sends and measure queue high-water mark/drop behavior.
7. Run the final product under weak-signal and congested-AP conditions.
8. For 64 channels, verify eight-device ordering and packet sequence continuity at the host.

## Board-specific items intentionally not fixed here

The SDK does not hard-code SSID/passwords, IP addresses, socket ports, ESP-IDF/lwIP/Realtek/Bouffalo APIs, task priorities, Wi-Fi power-save mode or antenna/radio configuration. Those belong in the selected board/application layer.
