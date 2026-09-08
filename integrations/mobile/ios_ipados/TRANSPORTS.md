# iOS / iPadOS Transport Integration

The Swift protocol layer is transport-independent. Transport adapters should deliver bytes/fragments without changing canonical packet semantics.

## CoreBluetooth / BLE

Firmware BLE fragmentation is defined in `firmware/transport/ble/`.

The Apple client must:

- discover/connect to the intended peripheral/service/characteristic defined by the actual firmware;
- treat BLE notifications as transport fragments, not automatically as complete EEG packets;
- parse the 8-byte repository BLE fragment header;
- enforce packet sequence, fragment index/count and payload length;
- reassemble exactly 49 canonical bytes;
- validate canonical CRC after reassembly;
- keep CoreBluetooth callbacks lightweight;
- move recording/rendering to an appropriate serialized pipeline.

Do not hard-code one MTU/application payload assumption across all iPhone/iPad models and BLE connections.

## Network.framework / TCP

For a raw canonical TCP stream:

- pass every received `Data` chunk to one `Ads1299StreamParser`;
- allow partial/multiple packets per receive callback;
- treat reconnect as an explicit session boundary;
- preserve parser and sequence statistics.

If the network source uses the repository Ethernet/Wi-Fi block framing, decode the block first, then validate each contained 49-byte packet.

## WebSocket gateway

A native Apple app may reuse the PWA gateway JSON model for development. In that mode:

- the gateway owns binary transport and canonical CRC validation;
- the app validates JSON message type/channel count/sequence/timestamp/status;
- multi-device identity must be explicit for 64 channels.

Do not run binary canonical and JSON gateway bytes through the same parser.

## Wired USB / external accessory path

A practical wired path depends on the hardware, USB role/accessory architecture and Apple platform support of the actual product. Keep the packet parser independent so any supported wired adapter only needs to deliver a byte stream.

Do not claim generic USB-host support for every iPhone/iPad and board without validating the exact accessory/device combination.

## UART through external bridge

If an Apple-accessible external bridge delivers the repository UART stream, remember that `firmware/transport/uart/` uses COBS + zero delimiter.

The adapter must:

1. delimit at `0x00`;
2. COBS-decode;
3. require one 49-byte canonical packet;
4. validate CRC;
5. track sequence gaps.

## Back-pressure

Use bounded queues between:

- BLE/network receive;
- protocol decode;
- file recording;
- UI rendering.

Expose:

- queue high-water mark;
- dropped buffers/packets;
- sequence gaps;
- reconnects;
- malformed fragment/message count.

Do not let UI frame-rate drops become silent raw-data drops.

## App lifecycle

If recording must continue while the app backgrounds or the screen locks, design and validate against the actual Apple background-execution model for the selected transport/use case.

Always represent interruptions in the session metadata. Do not synthesize continuity across an OS suspension gap.
