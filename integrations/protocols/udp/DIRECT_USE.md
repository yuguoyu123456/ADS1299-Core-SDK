# Direct use — UDP

## Sender

1. Build one valid `A9ET` block containing 1..16 canonical packets.
2. Send the entire block as one UDP datagram.
3. Increment `block_sequence` for each block.
4. Submit from a network task/queue, never from DRDY ISR context.

For synchronized 64-channel acquisition, a practical mapping is one 8-packet/408-byte datagram per sample epoch.

## Receiver

For each received datagram:

1. require at least the 16-byte `A9ET` header;
2. validate magic/version/count;
3. derive `expected_size = 16 + payload_bytes`;
4. require datagram length to equal `expected_size` exactly;
5. validate every 49-byte canonical packet;
6. use `block_sequence` to detect loss/reorder/duplicate blocks;
7. use canonical packet sequence numbers to detect acquisition-side gaps.

Do not concatenate two UDP datagrams to repair an incomplete application block.

## Loss handling

The application must define what happens when a datagram is missing.

For research recording, recommended behavior is usually:

- preserve sequence discontinuity;
- record an explicit missing-data event/count;
- avoid fabricating EEG samples;
- let later analysis decide whether/how to interpolate.

## Reordering

If out-of-order datagrams are acceptable, use a bounded reorder window. Do not create an unbounded map keyed by sequence.

If ultra-low latency is more important than reordering recovery, drop late datagrams after their epoch has already been emitted.

Document the chosen policy.

## Duplicate datagrams

Repeated `block_sequence` values should be recognized as duplicates according to the receiver policy. Do not append duplicate EEG epochs to a recording without marking them.

## Network jitter

Separate acquisition time from receive time:

- canonical `timestamp_us` = source/device timing;
- host receive timestamp = network/OS observation time.

Keep both if jitter analysis is important. Do not overwrite the device timestamp with UDP receive time.

## 64-channel systems

For 8×ADS1299:

- keep stable device/source mapping;
- verify all eight packets belong to the same acquisition epoch;
- retain per-device status/timestamp when available;
- do not infer synchronization from one datagram merely because packets were batched together.

## Security

Plain UDP does not provide authentication, confidentiality or retransmission. Use an appropriate secure network architecture when data leaves a trusted controlled environment.
