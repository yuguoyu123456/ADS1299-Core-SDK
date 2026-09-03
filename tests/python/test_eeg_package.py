"""Numerical smoke/regression tests for the installable ads1299_eeg package."""

from __future__ import annotations

import tempfile
from pathlib import Path

import numpy as np

from ads1299_eeg import (
    alpha_peak_frequency,
    assess_quality,
    bandpower,
    baseline_correct,
    basic_eeg_pipeline,
    codes_to_volts,
    coherence_matrix,
    decode_24be,
    epoch_data,
    lsb_volts,
    load_channel_csv,
    notch_filter,
    phase_locking_value,
    save_channel_csv,
    sign_extend_24,
    volts_to_codes,
    welch_psd,
)


def assert_close(actual, expected, tolerance, message):
    if abs(float(actual) - float(expected)) > tolerance:
        raise AssertionError(f"{message}: {actual!r} != {expected!r}")


def synthetic_eeg(fs: float = 250.0, seconds: float = 20.0) -> tuple[np.ndarray, np.ndarray]:
    rng = np.random.default_rng(42)
    t = np.arange(int(fs * seconds), dtype=float) / fs
    channels = []
    for index in range(8):
        phase = index * 0.19
        alpha = (18.0 + index) * np.sin(2 * np.pi * 10.0 * t + phase)
        theta = 3.0 * np.sin(2 * np.pi * 6.0 * t + phase / 2)
        line = 8.0 * np.sin(2 * np.pi * 50.0 * t)
        noise = rng.normal(0.0, 2.5, t.size)
        channels.append(alpha + theta + line + noise)
    return t, np.column_stack(channels)


def test_ads1299_conversion():
    assert sign_extend_24(0x000000) == 0
    assert sign_extend_24(0x000001) == 1
    assert sign_extend_24(0x7FFFFF) == 8388607
    assert sign_extend_24(0xFFFFFF) == -1
    assert sign_extend_24(0x800000) == -8388608
    assert decode_24be(bytes.fromhex("FFFFFF")) == -1

    expected_lsb = (2.0 * 4.5 / 24.0) / (2**24)
    assert_close(lsb_volts(4.5, 24), expected_lsb, 1e-18, "ADS1299 LSB")

    source = np.array([-50e-6, -1e-6, 0.0, 1e-6, 50e-6])
    codes = volts_to_codes(source, 4.5, 24)
    recovered = codes_to_volts(codes, 4.5, 24)
    if np.max(np.abs(recovered - source)) > expected_lsb:
        raise AssertionError("ADS1299 volts/code round trip exceeded one LSB")


def test_notch_and_spectral():
    fs = 250.0
    _, data = synthetic_eeg(fs)
    frequencies, psd_before = welch_psd(data, fs, nperseg=1000)
    filtered = notch_filter(data, fs, freq=50.0, q=30.0)
    _, psd_after = welch_psd(filtered, fs, nperseg=1000)
    line_index = int(np.argmin(np.abs(frequencies - 50.0)))
    attenuation = np.mean(psd_after[line_index]) / np.mean(psd_before[line_index])
    if not attenuation < 0.05:
        raise AssertionError(f"50-Hz notch attenuation too weak: ratio={attenuation}")

    peaks = alpha_peak_frequency(filtered, fs, nperseg=1000)
    if np.max(np.abs(peaks - 10.0)) > 0.5:
        raise AssertionError(f"alpha peak detection failed: {peaks}")

    powers = bandpower(filtered, fs, nperseg=1000)
    if not np.all(powers.relative["alpha"] > powers.relative["theta"]):
        raise AssertionError("synthetic alpha power should exceed theta power")
    if not np.all(powers.relative["alpha"] > powers.relative["beta"]):
        raise AssertionError("synthetic alpha power should exceed beta power")


def test_quality_report():
    fs = 250.0
    _, data = synthetic_eeg(fs, seconds=10.0)
    data[:, 7] = 0.0
    report = assess_quality(data, fs, max_line_noise_ratio=1.0)
    if 7 not in report.bad_channel_indices:
        raise AssertionError("flatline channel was not detected")
    if not any("flatline" in reason or "std" in reason for reason in report.channels[7].reasons):
        raise AssertionError(f"unexpected flatline reasons: {report.channels[7].reasons}")


def test_epochs_and_baseline():
    fs = 250.0
    _, data = synthetic_eeg(fs, seconds=12.0)
    events = [int(2 * fs), int(5 * fs), int(8 * fs)]
    epochs, times, kept = epoch_data(data, events, fs, -0.2, 0.8)
    if epochs.shape != (3, 250, 8):
        raise AssertionError(f"unexpected epoch shape: {epochs.shape}")
    if kept.tolist() != events:
        raise AssertionError("event bookkeeping changed")
    corrected = baseline_correct(epochs, times, (-0.2, 0.0))
    baseline_mask = (times >= -0.2) & (times <= 0.0)
    baseline_mean = np.mean(corrected[:, baseline_mask, :], axis=1)
    if np.max(np.abs(baseline_mean)) > 1e-10:
        raise AssertionError("baseline correction did not zero baseline means")


def test_connectivity():
    fs = 250.0
    _, data = synthetic_eeg(fs, seconds=10.0)
    coherence = coherence_matrix(data[:, :4], fs, band=(8.0, 13.0))
    plv = phase_locking_value(data[:, :4], fs, band=(8.0, 13.0))
    for matrix, name in [(coherence, "coherence"), (plv, "PLV")]:
        if matrix.shape != (4, 4):
            raise AssertionError(f"{name} shape wrong")
        if not np.allclose(matrix, matrix.T, equal_nan=True):
            raise AssertionError(f"{name} matrix must be symmetric")
        if not np.allclose(np.diag(matrix), 1.0):
            raise AssertionError(f"{name} diagonal must be one")


def test_csv_roundtrip():
    fs = 250.0
    t, data = synthetic_eeg(fs, seconds=1.0)
    with tempfile.TemporaryDirectory() as directory:
        path = Path(directory) / "recording.csv"
        names = [f"CH{i + 1}" for i in range(8)]
        save_channel_csv(path, data, channel_names=names, timestamps=t)
        recording = load_channel_csv(
            path,
            fs=fs,
            channel_columns=names,
            timestamp_column="timestamp",
        )
        if recording.data.shape != data.shape:
            raise AssertionError("CSV data shape changed")
        if not np.allclose(recording.data, data):
            raise AssertionError("CSV values changed")
        if not np.allclose(recording.timestamps, t):
            raise AssertionError("CSV timestamps changed")


def test_complete_pipeline():
    fs = 250.0
    _, data = synthetic_eeg(fs, seconds=20.0)
    result = basic_eeg_pipeline(
        data,
        fs,
        mains_frequency=50.0,
        low_cut_hz=0.5,
        high_cut_hz=45.0,
        apply_car=False,
    )
    if result.processed.shape != data.shape:
        raise AssertionError("pipeline changed channel/sample shape")
    if "alpha_peak_hz" not in result.features:
        raise AssertionError("pipeline did not expose alpha peak feature")
    if np.max(np.abs(result.features["alpha_peak_hz"] - 10.0)) > 0.5:
        raise AssertionError("pipeline alpha peak is incorrect")


def main():
    tests = [
        test_ads1299_conversion,
        test_notch_and_spectral,
        test_quality_report,
        test_epochs_and_baseline,
        test_connectivity,
        test_csv_roundtrip,
        test_complete_pipeline,
    ]
    for test in tests:
        test()
        print(f"PASS {test.__name__}")
    print(f"PASS {len(tests)} EEG package tests")


if __name__ == "__main__":
    main()
