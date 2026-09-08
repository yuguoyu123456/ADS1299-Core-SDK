"""Multi-ADS1299 epoch aggregation helpers for Linux edge gateways.

The aggregator requires the caller to provide an explicit device_index for each
canonical packet. It never infers device identity from arrival order.
"""

from __future__ import annotations

from collections import OrderedDict
from dataclasses import dataclass
from typing import Iterable

from canonical_packet import CanonicalPacket

CHANNELS_PER_DEVICE = 8


@dataclass(frozen=True)
class MultiDeviceEpoch:
    sequence: int
    device_count: int
    timestamp_us: tuple[int, ...]
    status_hex: tuple[str, ...]
    flags: tuple[int, ...]
    channels: tuple[int, ...]


class MultiDeviceEpochAssembler:
    """Collect one packet from each device for an exact shared sequence number.

    This is appropriate when all ADS1299 acquisition contexts are configured to
    emit synchronized, matching sequence numbers. Arrival order may differ.
    Pending epochs are bounded so a stalled/missing device cannot grow memory
    without limit.
    """

    def __init__(self, device_count: int, max_pending_epochs: int = 32) -> None:
        if device_count <= 0:
            raise ValueError("device_count must be positive")
        if max_pending_epochs <= 0:
            raise ValueError("max_pending_epochs must be positive")
        self.device_count = device_count
        self.max_pending_epochs = max_pending_epochs
        self._pending: OrderedDict[int, dict[int, CanonicalPacket]] = OrderedDict()
        self.completed_epochs = 0
        self.evicted_epochs = 0
        self.duplicate_device_packets = 0

    def push(self, device_index: int, packet: CanonicalPacket) -> MultiDeviceEpoch | None:
        if not 0 <= device_index < self.device_count:
            raise ValueError("device_index out of range")

        bucket = self._pending.get(packet.sequence)
        if bucket is None:
            bucket = {}
            self._pending[packet.sequence] = bucket

        if device_index in bucket:
            self.duplicate_device_packets += 1
        bucket[device_index] = packet

        while len(self._pending) > self.max_pending_epochs:
            self._pending.popitem(last=False)
            self.evicted_epochs += 1

        if len(bucket) != self.device_count:
            return None

        ordered = [bucket[i] for i in range(self.device_count)]
        del self._pending[packet.sequence]
        self.completed_epochs += 1
        return MultiDeviceEpoch(
            sequence=packet.sequence,
            device_count=self.device_count,
            timestamp_us=tuple(p.timestamp_us for p in ordered),
            status_hex=tuple(p.status.hex().upper() for p in ordered),
            flags=tuple(p.flags for p in ordered),
            channels=tuple(value for p in ordered for value in p.channels),
        )

    @property
    def pending_epochs(self) -> int:
        return len(self._pending)


def global_channel_index(device_index: int, local_channel_index: int) -> int:
    if device_index < 0:
        raise ValueError("device_index must be non-negative")
    if not 0 <= local_channel_index < CHANNELS_PER_DEVICE:
        raise ValueError("local_channel_index out of range")
    return device_index * CHANNELS_PER_DEVICE + local_channel_index


def flatten_channels(packets: Iterable[CanonicalPacket]) -> tuple[int, ...]:
    return tuple(value for packet in packets for value in packet.channels)
