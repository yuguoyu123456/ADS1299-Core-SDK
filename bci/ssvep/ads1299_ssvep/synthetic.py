"""Synthetic SSVEP signals for examples and numerical regression tests."""

from __future__ import annotations

from collections.abc import Sequence

import numpy as np

from .core import normalize_frequencies, validate_fs


def simulate_ssvep_window(
    frequency_hz: float,
    fs: float = 250.0,
    duration_s: float = 2.0,
    *,
    channels: int = 4,
    harmonics: int = 3,
    signal_uv: float = 12.0,
    noise_uv: float = 2.0,
    line_hz: float | None = 50.0,
    line_uv: float = 1.0,
    seed: int | None = None,
) -> np.ndarray:
    """Generate a reproducible multi-channel SSVEP-like EEG window."""
    fs = validate_fs(fs)
    f = float(frequency_hz)
    if not 0 < f < fs / 2:
        raise ValueError("frequency_hz must lie inside (0, Nyquist)")
    channels = int(channels)
    harmonics = int(harmonics)
    n = int(round(float(duration_s) * fs))
    if channels < 1 or harmonics < 1 or n < 16:
        raise ValueError("invalid channels, harmonics or duration")
    if signal_uv <= 0 or noise_uv < 0 or line_uv < 0:
        raise ValueError("signal/noise amplitudes must be non-negative and signal positive")

    rng = np.random.default_rng(seed)
    t = np.arange(n, dtype=float) / fs
    data = np.zeros((n, channels), dtype=float)
    for ch in range(channels):
        gain = float(signal_uv) * rng.uniform(0.75, 1.25)
        phase = rng.uniform(-np.pi, np.pi)
        for h in range(1, harmonics + 1):
            hf = h * f
            if hf >= fs / 2:
                break
            harmonic_gain = gain / (h ** 1.15)
            data[:, ch] += harmonic_gain * np.sin(2 * np.pi * hf * t + h * phase)
        if line_hz is not None and 0 < float(line_hz) < fs / 2 and line_uv > 0:
            data[:, ch] += float(line_uv) * np.sin(
                2 * np.pi * float(line_hz) * t + rng.uniform(-np.pi, np.pi)
            )
        if noise_uv > 0:
            data[:, ch] += rng.normal(0.0, float(noise_uv), n)
    return data


def simulate_ssvep_dataset(
    frequencies: Sequence[float],
    *,
    trials_per_class: int = 8,
    fs: float = 250.0,
    duration_s: float = 2.0,
    channels: int = 4,
    harmonics: int = 3,
    signal_uv: float = 12.0,
    noise_uv: float = 2.0,
    seed: int = 7,
) -> tuple[np.ndarray, np.ndarray]:
    """Return ``(epochs, labels)`` with epochs shaped trials x samples x channels."""
    freqs = normalize_frequencies(frequencies, fs)
    trials_per_class = int(trials_per_class)
    if trials_per_class < 2:
        raise ValueError("trials_per_class must be at least two")
    epochs: list[np.ndarray] = []
    labels: list[float] = []
    trial_seed = int(seed)
    for freq in freqs:
        for _ in range(trials_per_class):
            epochs.append(
                simulate_ssvep_window(
                    freq,
                    fs,
                    duration_s,
                    channels=channels,
                    harmonics=harmonics,
                    signal_uv=signal_uv,
                    noise_uv=noise_uv,
                    seed=trial_seed,
                )
            )
            labels.append(freq)
            trial_seed += 1
    return np.stack(epochs, axis=0), np.asarray(labels, dtype=float)
