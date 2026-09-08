#!/usr/bin/env python3
"""Dependency-light visual stimulus event marker helper.

Records host timing plus optional display/physical-onset/EEG references without
claiming that software dispatch time equals photon onset at the eye.
"""

from __future__ import annotations

from dataclasses import asdict, dataclass
import json
from pathlib import Path
import time
from typing import Any, Optional


@dataclass(frozen=True)
class VisualEventMarker:
    event_index: int
    event_name: str
    stimulus_id: str
    condition: str
    host_monotonic_ns: int
    host_unix_ns: int
    intended_frame_index: Optional[int] = None
    presentation_time_ns: Optional[int] = None
    physical_onset_time_ns: Optional[int] = None
    eeg_sequence: Optional[int] = None
    eeg_timestamp_us: Optional[int] = None
    metadata: Optional[dict[str, Any]] = None


def make_marker(
    event_index: int,
    event_name: str,
    stimulus_id: str,
    condition: str,
    *,
    intended_frame_index: Optional[int] = None,
    presentation_time_ns: Optional[int] = None,
    physical_onset_time_ns: Optional[int] = None,
    eeg_sequence: Optional[int] = None,
    eeg_timestamp_us: Optional[int] = None,
    metadata: Optional[dict[str, Any]] = None,
) -> VisualEventMarker:
    if event_index < 0:
        raise ValueError("event_index must be non-negative")
    if not event_name or not stimulus_id:
        raise ValueError("event_name and stimulus_id must not be empty")
    if intended_frame_index is not None and intended_frame_index < 0:
        raise ValueError("intended_frame_index must be non-negative")
    if eeg_sequence is not None and not 0 <= eeg_sequence <= 0xFFFFFFFF:
        raise ValueError("eeg_sequence must fit uint32")
    if eeg_timestamp_us is not None and not 0 <= eeg_timestamp_us <= 0xFFFFFFFF:
        raise ValueError("eeg_timestamp_us must fit uint32")

    return VisualEventMarker(
        event_index=event_index,
        event_name=event_name,
        stimulus_id=stimulus_id,
        condition=condition,
        host_monotonic_ns=time.monotonic_ns(),
        host_unix_ns=time.time_ns(),
        intended_frame_index=intended_frame_index,
        presentation_time_ns=presentation_time_ns,
        physical_onset_time_ns=physical_onset_time_ns,
        eeg_sequence=eeg_sequence,
        eeg_timestamp_us=eeg_timestamp_us,
        metadata=metadata,
    )


def append_jsonl(path: str | Path, marker: VisualEventMarker) -> None:
    target = Path(path)
    target.parent.mkdir(parents=True, exist_ok=True)
    with target.open("a", encoding="utf-8") as f:
        json.dump(asdict(marker), f, ensure_ascii=False, separators=(",", ":"))
        f.write("\n")
