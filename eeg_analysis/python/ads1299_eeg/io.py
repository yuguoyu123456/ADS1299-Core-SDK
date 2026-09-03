"""Lightweight host-side I/O helpers for EEG recordings.

The package intentionally avoids a pandas dependency in its core. CSV helpers
therefore use Python's standard library and NumPy. For larger research data,
MNE-Python/EDF/BDF/FIF or another established scientific format is preferred.
"""

from __future__ import annotations

import csv
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

import numpy as np

from .core import default_channel_names


@dataclass(frozen=True)
class ContinuousRecording:
    data: np.ndarray
    fs: float
    channel_names: tuple[str, ...]
    timestamps: np.ndarray | None = None

    @property
    def duration_seconds(self) -> float:
        return self.data.shape[0] / self.fs


def load_channel_csv(
    path: str | Path,
    *,
    fs: float,
    channel_columns: Sequence[str] | None = None,
    timestamp_column: str | None = None,
    delimiter: str = ",",
) -> ContinuousRecording:
    """Load a header-based CSV file into ``samples x channels`` data.

    Parameters
    ----------
    channel_columns:
        Explicit channel column names. When omitted, columns whose names begin
        with ``ch`` (case-insensitive) are selected in file order.
    timestamp_column:
        Optional timestamp column retained as a separate array.
    """
    path = Path(path)
    if fs <= 0:
        raise ValueError("fs must be positive")
    with path.open("r", encoding="utf-8-sig", newline="") as handle:
        reader = csv.DictReader(handle, delimiter=delimiter)
        if not reader.fieldnames:
            raise ValueError("CSV requires a header row")
        fieldnames = list(reader.fieldnames)
        if channel_columns is None:
            selected = [name for name in fieldnames if name.lower().startswith("ch")]
        else:
            selected = list(channel_columns)
        if not selected:
            raise ValueError("no EEG channel columns selected")
        missing = [name for name in selected if name not in fieldnames]
        if missing:
            raise ValueError(f"missing CSV channel columns: {missing}")
        if timestamp_column is not None and timestamp_column not in fieldnames:
            raise ValueError(f"missing timestamp column: {timestamp_column}")

        rows: list[list[float]] = []
        timestamps: list[float] = []
        for row_number, row in enumerate(reader, start=2):
            try:
                rows.append([float(row[name]) for name in selected])
                if timestamp_column is not None:
                    timestamps.append(float(row[timestamp_column]))
            except (TypeError, ValueError) as exc:
                raise ValueError(f"non-numeric value in CSV row {row_number}") from exc

    if not rows:
        raise ValueError("CSV contains no data rows")
    data = np.asarray(rows, dtype=float)
    ts = np.asarray(timestamps, dtype=float) if timestamp_column is not None else None
    return ContinuousRecording(data, float(fs), tuple(selected), ts)


def save_channel_csv(
    path: str | Path,
    data: np.ndarray,
    *,
    channel_names: Sequence[str] | None = None,
    timestamps: np.ndarray | None = None,
    delimiter: str = ",",
) -> None:
    """Write ``samples x channels`` data to a simple interoperable CSV file."""
    x = np.asarray(data, dtype=float)
    if x.ndim == 1:
        x = x[:, None]
    if x.ndim != 2:
        raise ValueError("data must be 1-D or samples x channels")
    names = default_channel_names(x.shape[1]) if channel_names is None else list(channel_names)
    if len(names) != x.shape[1]:
        raise ValueError("channel_names length must match channel count")

    ts = None
    if timestamps is not None:
        ts = np.asarray(timestamps, dtype=float)
        if ts.ndim != 1 or ts.size != x.shape[0]:
            raise ValueError("timestamps must be 1-D and match sample count")

    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.writer(handle, delimiter=delimiter)
        header = (["timestamp"] if ts is not None else []) + names
        writer.writerow(header)
        for index in range(x.shape[0]):
            row: list[float] = []
            if ts is not None:
                row.append(float(ts[index]))
            row.extend(float(value) for value in x[index])
            writer.writerow(row)


def infer_uniform_sampling_rate(timestamps: np.ndarray, *, tolerance: float = 0.01) -> float:
    """Estimate sample rate and reject strongly non-uniform timestamps.

    ``tolerance`` is the maximum relative median absolute deviation of sample
    intervals. This is intended as a sanity check, not clock-drift calibration.
    """
    ts = np.asarray(timestamps, dtype=float)
    if ts.ndim != 1 or ts.size < 3:
        raise ValueError("at least three timestamps are required")
    intervals = np.diff(ts)
    if np.any(~np.isfinite(intervals)) or np.any(intervals <= 0):
        raise ValueError("timestamps must be finite and strictly increasing")
    median = float(np.median(intervals))
    mad = float(np.median(np.abs(intervals - median)))
    if median <= 0:
        raise ValueError("invalid timestamp spacing")
    if mad / median > tolerance:
        raise ValueError("timestamps are too irregular for a uniform sample-rate assumption")
    return 1.0 / median
