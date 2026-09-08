#!/usr/bin/env python3
"""Dependency-light JSONL event marker helper for audio/EEG experiments.

This module records host monotonic timing and optional experiment/audio timing
metadata without pretending software dispatch time is identical to acoustic
onset at the ear.
"""

from __future__ import annotations

from dataclasses import asdict, dataclass
import json
from pathlib import Path
import time
from typing import Any, Optional


@dataclass(frozen=True)
class AudioEventMarker:
    event_index: int
    event_name: str
    stimulus_id: str
    host_monotonic_ns: int
    host_unix_ns: int
    audio_time_ns: Optional[int] = None
    physical_onset_time_ns: Optional[int] = None
    eeg_sequence: Optional[int] = None
    eeg_timestamp_us: Optional[int] = None
    metadata: Optional[dict[str, Any]] = None


def make_marker(
    event_index: int,
    event_name: str,
    stimulus_id: str,
    *,
    audio_time_ns: Optional[int] = None,
    physical_onset_time_ns: Optional[int] = None,
    eeg_sequence: Optional[int] = None,
    eeg_timestamp_us: Optional[int] = None,
    metadata: Optional[dict[str, Any]] = None,
) -> AudioEventMarker:
    if event_index < 0:
        raise ValueError("event_index must be non-negative")
    if not event_name:
        raise ValueError("event_name must not be empty")
    if not stimulus_id:
        raise ValueError("stimulus_id must not be empty")
    if eeg_sequence is not None and not 0 <= eeg_sequence <= 0xFFFFFFFF:
        raise ValueError("eeg_sequence must fit uint32")
    if eeg_timestamp_us is not None and not 0 <= eeg_timestamp_us <= 0xFFFFFFFF:
        raise ValueError("eeg_timestamp_us must fit uint32")

    return AudioEventMarker(
        event_index=event_index,
        event_name=event_name,
        stimulus_id=stimulus_id,
        host_monotonic_ns=time.monotonic_ns(),
        host_unix_ns=time.time_ns(),
        audio_time_ns=audio_time_ns,
        physical_onset_time_ns=physical_onset_time_ns,
        eeg_sequence=eeg_sequence,
        eeg_timestamp_us=eeg_timestamp_us,
        metadata=metadata,
    )


def append_jsonl(path: str | Path, marker: AudioEventMarker) -> None:
    target = Path(path)
    target.parent.mkdir(parents=True, exist_ok=True)
    with target.open("a", encoding="utf-8") as f:
        json.dump(asdict(marker), f, ensure_ascii=False, separators=(",", ":"))
        f.write("\n")


def load_jsonl(path: str | Path) -> list[dict[str, Any]]:
    records: list[dict[str, Any]] = []
    with Path(path).open("r", encoding="utf-8") as f:
        for line_number, line in enumerate(f, start=1):
            line = line.strip()
            if not line:
                continue
            value = json.loads(line)
            if not isinstance(value, dict):
                raise ValueError(f"line {line_number}: expected JSON object")
            records.append(value)
    return records


if __name__ == "__main__":
    marker = make_marker(
        0,
        "stimulus_start",
        "example-tone",
        metadata={"note": "software marker example; not physical acoustic onset"},
    )
    print(json.dumps(asdict(marker), indent=2, ensure_ascii=False))
