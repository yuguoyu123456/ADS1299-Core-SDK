# Direct use — TCP

## Sender

1. Encode/validate one or more canonical 49-byte ADS1299 packets.
2. Group 1..16 packets into one Ethernet/TCP block.
3. Increment `block_sequence` per application block.
4. Submit the complete block from a network/transport task, not from DRDY ISR context.
5. Handle partial socket writes until the whole block is accepted or the connection fails.

For a synchronized 64-channel epoch, eight canonical packets may be grouped into one 408-byte block.

## Receiver

Treat TCP as an arbitrary byte stream.

Recommended state machine:

```text
READ_HEADER(16)
  -> validate magic/version/count/length
  -> READ_PAYLOAD(payload_bytes)
  -> validate every 49-byte canonical packet
  -> deliver packets
  -> READ_HEADER(16)
```

If the connection closes mid-block, discard the incomplete block. Do not concatenate bytes from a new connection onto old partial state.

## Resynchronization

For a healthy TCP connection, block boundaries should be recoverable from the fixed header and declared payload size. If an application chooses to recover after malformed data rather than closing the connection, search for the next `A9ET` magic candidate and then validate all header constraints before trusting it.

## Backpressure

A slow receiver can cause socket sends to block or return partial progress. Therefore:

- acquisition must enqueue into a bounded transport queue;
- the socket task owns retries/partial writes;
- queue overflow policy must be explicit;
- sequence gaps must remain observable;
- unbounded buffering is not acceptable.

## Reconnect

On reconnect:

- clear partial block state;
- optionally reset a connection-local block counter, but preserve sample-level sequence semantics if the acquisition source continues running;
- expose a discontinuity/reconnect event to downstream recording when appropriate.

## Security

Plain TCP framing is not encryption or authentication. For untrusted networks, place it inside an authenticated/encrypted transport or use an application gateway that provides the required security controls.

## 64-channel note

When eight devices are grouped per epoch, keep a deterministic device ordering in the block payload or carry explicit source metadata at the integration layer. Do not infer device identity from network timing.
