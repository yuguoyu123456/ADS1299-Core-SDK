#!/usr/bin/env python3
"""Smoke test for event_marker.py using only the Python standard library."""

from __future__ import annotations

import json
from pathlib import Path
import tempfile

from event_marker import append_jsonl, load_jsonl, make_marker


def main() -> None:
    marker0 = make_marker(
        0,
        "stimulus_start",
        "tone-1000hz",
        audio_time_ns=123456789,
        eeg_sequence=42,
        eeg_timestamp_us=100000,
        metadata={"condition": "A"},
    )
    marker1 = make_marker(
        1,
        "stimulus_stop",
        "tone-1000hz",
        physical_onset_time_ns=223456789,
    )

    assert marker0.event_index == 0
    assert marker0.host_monotonic_ns > 0
    assert marker0.host_unix_ns > 0
    assert marker0.eeg_sequence == 42

    try:
        make_marker(-1, "bad", "x")
        raise AssertionError("negative event index was accepted")
    except ValueError:
        pass

    try:
        make_marker(2, "bad-seq", "x", eeg_sequence=0x1_0000_0000)
        raise AssertionError("out-of-range EEG sequence was accepted")
    except ValueError:
        pass

    with tempfile.TemporaryDirectory() as tmp:
        path = Path(tmp) / "events.jsonl"
        append_jsonl(path, marker0)
        append_jsonl(path, marker1)
        records = load_jsonl(path)
        assert len(records) == 2
        assert records[0]["stimulus_id"] == "tone-1000hz"
        assert records[0]["metadata"]["condition"] == "A"
        assert records[1]["physical_onset_time_ns"] == 223456789

        # Every non-empty line should be one valid JSON object.
        lines = [line for line in path.read_text(encoding="utf-8").splitlines() if line]
        assert len(lines) == 2
        assert all(isinstance(json.loads(line), dict) for line in lines)

    print("audio event marker smoke test: PASS")


if __name__ == "__main__":
    main()
