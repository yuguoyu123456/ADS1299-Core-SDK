#!/usr/bin/env python3
"""Minimal vendor-neutral audio endpoint adapter contract.

The contract exposes only software-control facts. It deliberately does not
claim physical acoustic onset timing or undocumented consumer-device access.
"""

from __future__ import annotations

from dataclasses import dataclass
from typing import Protocol, Optional
import time


@dataclass(frozen=True)
class AudioEndpointInfo:
    endpoint_id: str
    display_name: str
    adapter_name: str
    capabilities: tuple[str, ...]


@dataclass(frozen=True)
class AudioDispatchResult:
    accepted: bool
    event_index: int
    stimulus_id: str
    host_dispatch_monotonic_ns: int
    endpoint_time_ns: Optional[int] = None
    detail: str = ""


class AudioEndpointAdapter(Protocol):
    def connect(self) -> AudioEndpointInfo:
        ...

    def disconnect(self) -> None:
        ...

    def play(self, event_index: int, stimulus_id: str) -> AudioDispatchResult:
        ...

    def stop(self, event_index: int, stimulus_id: str) -> AudioDispatchResult:
        ...


class MockAudioEndpoint:
    """Test-only adapter demonstrating the contract without external SDKs."""

    def __init__(self, endpoint_id: str = "mock-0") -> None:
        self.endpoint_id = endpoint_id
        self.connected = False

    def connect(self) -> AudioEndpointInfo:
        self.connected = True
        return AudioEndpointInfo(
            endpoint_id=self.endpoint_id,
            display_name="Mock Audio Endpoint",
            adapter_name="mock",
            capabilities=("play", "stop"),
        )

    def disconnect(self) -> None:
        self.connected = False

    def _result(self, event_index: int, stimulus_id: str, action: str) -> AudioDispatchResult:
        if event_index < 0:
            raise ValueError("event_index must be non-negative")
        if not stimulus_id:
            raise ValueError("stimulus_id must not be empty")
        if not self.connected:
            return AudioDispatchResult(
                accepted=False,
                event_index=event_index,
                stimulus_id=stimulus_id,
                host_dispatch_monotonic_ns=time.monotonic_ns(),
                detail="endpoint not connected",
            )
        return AudioDispatchResult(
            accepted=True,
            event_index=event_index,
            stimulus_id=stimulus_id,
            host_dispatch_monotonic_ns=time.monotonic_ns(),
            endpoint_time_ns=None,
            detail=f"mock {action} accepted; not a physical-onset measurement",
        )

    def play(self, event_index: int, stimulus_id: str) -> AudioDispatchResult:
        return self._result(event_index, stimulus_id, "play")

    def stop(self, event_index: int, stimulus_id: str) -> AudioDispatchResult:
        return self._result(event_index, stimulus_id, "stop")
