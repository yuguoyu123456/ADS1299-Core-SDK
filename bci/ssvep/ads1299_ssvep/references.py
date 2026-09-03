"""Sinusoidal reference construction for training-free SSVEP classifiers."""

from __future__ import annotations

import numpy as np

from .core import validate_fs


def harmonic_references(
    frequency_hz: float,
    fs: float,
    n_samples: int,
    *,
    harmonics: int = 3,
    phase_offset_rad: float = 0.0,
) -> np.ndarray:
    """Build sin/cos references for harmonics that remain below Nyquist.

    The output shape is ``samples x (2 * usable_harmonics)``. Harmonics that
    would equal or exceed Nyquist are omitted instead of silently aliasing.
    """
    fs = validate_fs(fs)
    f = float(frequency_hz)
    n_samples = int(n_samples)
    harmonics = int(harmonics)
    if not np.isfinite(f) or f <= 0 or f >= fs / 2:
        raise ValueError("frequency_hz must lie inside (0, Nyquist)")
    if n_samples < 4:
        raise ValueError("n_samples must be at least four")
    if harmonics < 1:
        raise ValueError("harmonics must be at least one")

    t = np.arange(n_samples, dtype=float) / fs
    columns: list[np.ndarray] = []
    for h in range(1, harmonics + 1):
        hf = h * f
        if hf >= fs / 2:
            break
        angle = 2.0 * np.pi * hf * t + h * float(phase_offset_rad)
        columns.append(np.sin(angle))
        columns.append(np.cos(angle))
    if not columns:
        raise ValueError("no requested harmonic remains below Nyquist")
    return np.column_stack(columns)
