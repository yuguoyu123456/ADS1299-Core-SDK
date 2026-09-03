"""Optional MNE-Python interoperability example.

Install separately: pip install mne
This file is intentionally not part of the lightweight CI dependency set.
"""

from __future__ import annotations
import numpy as np


def to_mne_raw(eeg_uv: np.ndarray, fs: float, channel_names=None):
    import mne

    x = np.asarray(eeg_uv, dtype=float)
    if x.ndim != 2:
        raise ValueError("Expected samples x channels")
    if channel_names is None:
        channel_names = [f"EEG{i+1}" for i in range(x.shape[1])]
    if len(channel_names) != x.shape[1]:
        raise ValueError("channel_names length mismatch")

    info = mne.create_info(channel_names, sfreq=fs, ch_types="eeg")
    # MNE EEG convention is volts and channels x samples.
    raw = mne.io.RawArray((x.T * 1e-6), info, verbose=False)
    return raw
