"""Stable JSON-serializable message builders for ADS1299 edge gateways."""

from __future__ import annotations

from typing import Any

from canonical_packet import CanonicalPacket
from multi_device import MultiDeviceEpoch


def hello_message(channels: int, source_count: int = 1) -> dict[str, Any]:
    if channels <= 0 or source_count <= 0:
        raise ValueError("channels and source_count must be positive")
    return {
        "type": "hello",
        "project": "ADS1299-Core-SDK",
        "transport": "websocket-json-v1",
        "channels": channels,
        "source_count": source_count,
        "canonical_packet_bytes": 49,
    }


def packet_message(packet: CanonicalPacket, device_index: int = 0) -> dict[str, Any]:
    if device_index < 0:
        raise ValueError("device_index must be non-negative")
    return {
        "type": "eeg",
        "protocol_version": packet.version,
        "device_index": device_index,
        "flags": packet.flags,
        "sequence": packet.sequence,
        "timestamp_us": packet.timestamp_us,
        "status_hex": packet.status.hex().upper(),
        "channels": list(packet.channels),
    }


def epoch_message(epoch: MultiDeviceEpoch) -> dict[str, Any]:
    return {
        "type": "eeg_epoch",
        "sequence": epoch.sequence,
        "device_count": epoch.device_count,
        "channel_count": len(epoch.channels),
        "timestamps_us": list(epoch.timestamp_us),
        "status_hex": list(epoch.status_hex),
        "flags": list(epoch.flags),
        "channels": list(epoch.channels),
        "channel_mapping": "global=device_index*8+local_channel",
    }
