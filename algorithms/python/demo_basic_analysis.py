"""Synthetic-data demo for the ADS1299-Core-SDK EEG starter algorithms.

Run:
    python demo_basic_analysis.py

The demo deliberately uses synthetic signals so it can be executed before real
hardware is available. It generates 8 channels containing alpha activity,
50-Hz mains noise and random noise, then runs a basic preprocessing + PSD +
feature workflow.
"""

from __future__ import annotations

import numpy as np

from eeg_basics import (
    alpha_peak,
    bandpass_filter,
    bandpower,
    common_average_reference,
    hjorth_parameters,
    notch_filter,
    time_domain_features,
    welch_psd,
)


def make_synthetic_eeg(fs: int = 250, seconds: int = 20, seed: int = 7):
    rng = np.random.default_rng(seed)
    t = np.arange(fs * seconds) / fs
    channels = []
    for ch in range(8):
        alpha_hz = 9.5 + 0.2 * ch
        alpha = (15 + ch) * np.sin(2 * np.pi * alpha_hz * t)
        theta = 5 * np.sin(2 * np.pi * 6 * t + 0.15 * ch)
        mains = 8 * np.sin(2 * np.pi * 50 * t)
        noise = rng.normal(0, 6, size=t.shape)
        channels.append(alpha + theta + mains + noise)
    return t, np.column_stack(channels)


def main():
    fs = 250
    _, raw_uv = make_synthetic_eeg(fs=fs)

    x = common_average_reference(raw_uv)
    x = notch_filter(x, fs=fs, freq=50.0, q=30.0)
    x = bandpass_filter(x, fs=fs, low=0.5, high=45.0)

    freqs, psd = welch_psd(x, fs=fs)
    bp = bandpower(x, fs=fs)
    peak = alpha_peak(x, fs=fs)
    td = time_domain_features(x)
    hj = hjorth_parameters(x)

    print("ADS1299-Core-SDK synthetic EEG demo")
    print(f"Data shape: {x.shape} [samples, channels]")
    print(f"PSD shape: {psd.shape} [frequency bins, channels]")
    print(f"Frequency range: {freqs[0]:.2f}–{freqs[-1]:.2f} Hz")
    print("Alpha peak per channel (Hz):", np.round(peak, 2))
    print("Relative alpha power:", np.round(bp.relative["alpha"], 3))
    print("RMS (uV):", np.round(td["rms"], 2))
    print("Hjorth mobility:", np.round(hj["mobility"], 3))


if __name__ == "__main__":
    main()
