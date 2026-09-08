#!/usr/bin/env python3
"""Smoke test for visual_event_marker.py."""

from pathlib import Path
import json
import tempfile

from visual_event_marker import append_jsonl, make_marker


def main() -> None:
    marker = make_marker(
        0,
        "stimulus_onset",
        "checkerboard-a",
        "target",
        intended_frame_index=120,
        presentation_time_ns=123456789,
        eeg_sequence=42,
        eeg_timestamp_us=98765,
        metadata={"paradigm": "SSVEP"},
    )
    assert marker.event_index == 0
    assert marker.intended_frame_index == 120
    assert marker.eeg_sequence == 42
    assert marker.host_monotonic_ns > 0

    with tempfile.TemporaryDirectory() as tmp:
        path = Path(tmp) / "visual_events.jsonl"
        append_jsonl(path, marker)
        lines = path.read_text(encoding="utf-8").splitlines()
        assert len(lines) == 1
        value = json.loads(lines[0])
        assert value["stimulus_id"] == "checkerboard-a"
        assert value["condition"] == "target"
        assert value["metadata"]["paradigm"] == "SSVEP"

    try:
        make_marker(-1, "x", "y", "z")
        raise AssertionError("negative event index accepted")
    except ValueError:
        pass

    try:
        make_marker(1, "x", "y", "z", eeg_sequence=0x1_0000_0000)
        raise AssertionError("out-of-range sequence accepted")
    except ValueError:
        pass

    print("visual event marker smoke test: PASS")


if __name__ == "__main__":
    main()
