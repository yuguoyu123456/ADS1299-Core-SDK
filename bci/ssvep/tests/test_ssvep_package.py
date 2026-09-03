"""Numerical regression tests for the ADS1299 SSVEP package.

Run directly after installing the package:
    python bci/ssvep/tests/test_ssvep_package.py
"""

from __future__ import annotations

import numpy as np

from ads1299_ssvep import (
    CCAClassifier,
    FBCCAClassifier,
    SSVEPStreamClassifier,
    TRCAClassifier,
    accuracy_score,
    canonical_correlation,
    confusion_matrix,
    decision_from_scores,
    frequency_snr_db,
    harmonic_references,
    itr_bits_per_minute,
    simulate_ssvep_dataset,
    simulate_ssvep_window,
)


def assert_close(actual: float, expected: float, tol: float, message: str = "") -> None:
    if not abs(float(actual) - float(expected)) <= float(tol):
        raise AssertionError(message or f"{actual} is not within {tol} of {expected}")


def test_harmonic_references() -> None:
    ref = harmonic_references(12.0, 250.0, 500, harmonics=3)
    assert ref.shape == (500, 6)
    assert np.all(np.isfinite(ref))
    assert np.max(np.abs(np.mean(ref, axis=0))) < 0.02

    # At fs=100, 20-Hz references may use 20 and 40 Hz but must omit 60 Hz.
    clipped = harmonic_references(20.0, 100.0, 300, harmonics=4)
    assert clipped.shape == (300, 4)


def test_canonical_correlation_invariants() -> None:
    fs = 250.0
    ref = harmonic_references(12.0, fs, 500, harmonics=2)
    # A full-rank linear remix spans the same subspace, so largest CCA is ~1.
    remix = ref @ np.array(
        [
            [1.0, 0.2, 0.0, 0.1],
            [0.1, 1.1, 0.3, 0.0],
            [0.2, 0.0, 0.9, 0.1],
            [0.0, 0.2, 0.1, 1.0],
        ]
    )
    rho = canonical_correlation(ref, remix)
    assert 0.999 < rho <= 1.0


def test_cca_multiple_targets() -> None:
    fs = 250.0
    targets = [8.0, 10.0, 12.0, 15.0]
    classifier = CCAClassifier(targets, fs, harmonics=3)
    for seed, target in enumerate(targets, start=100):
        eeg = simulate_ssvep_window(
            target,
            fs,
            2.0,
            channels=4,
            harmonics=3,
            signal_uv=12.0,
            noise_uv=2.5,
            line_uv=1.0,
            seed=seed,
        )
        decision = classifier.predict(eeg)
        assert decision.label_hz == target, (target, decision)
        assert decision.best_score > 0.65
        assert decision.margin > 0.05


def test_spectral_snr_prefers_target() -> None:
    fs = 250.0
    eeg = simulate_ssvep_window(
        12.0,
        fs,
        3.0,
        channels=4,
        signal_uv=15.0,
        noise_uv=2.0,
        line_uv=0.5,
        seed=17,
    )
    target = frequency_snr_db(eeg, fs, 12.0, harmonics=3)
    nontarget = frequency_snr_db(eeg, fs, 10.0, harmonics=3)
    assert target > nontarget + 6.0, (target, nontarget)
    assert target > 8.0


def test_fbcca_target_recovery() -> None:
    fs = 250.0
    targets = [8.0, 10.0, 12.0, 15.0]
    classifier = FBCCAClassifier(targets, fs, harmonics=3)
    for seed, target in [(23, 10.0), (24, 12.0), (25, 15.0)]:
        eeg = simulate_ssvep_window(
            target,
            fs,
            3.0,
            channels=4,
            harmonics=3,
            signal_uv=15.0,
            noise_uv=1.5,
            line_uv=0.5,
            seed=seed,
        )
        decision = classifier.predict(eeg)
        assert decision.label_hz == target, (target, decision)
        assert decision.margin > 0.05


def test_rejection_logic() -> None:
    accepted = decision_from_scores({8.0: 0.8, 10.0: 0.4}, min_score=0.5, min_margin=0.1)
    assert not accepted.rejected
    assert accepted.label_hz == 8.0

    weak = decision_from_scores({8.0: 0.30, 10.0: 0.25}, min_score=0.5)
    assert weak.rejected and weak.label_hz is None

    ambiguous = decision_from_scores({8.0: 0.80, 10.0: 0.78}, min_margin=0.05)
    assert ambiguous.rejected and ambiguous.label_hz is None


def _split_trca_dataset() -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    targets = [8.0, 10.0, 12.0, 15.0]
    epochs, labels = simulate_ssvep_dataset(
        targets,
        trials_per_class=7,
        fs=250.0,
        duration_s=2.0,
        channels=4,
        harmonics=3,
        signal_uv=12.0,
        noise_uv=1.5,
        phase_locked=True,
        seed=31,
    )
    train_idx: list[int] = []
    test_idx: list[int] = []
    for target in targets:
        idx = np.flatnonzero(labels == target)
        train_idx.extend(idx[:5].tolist())
        test_idx.extend(idx[5:].tolist())
    return epochs[train_idx], labels[train_idx], epochs[test_idx], labels[test_idx]


def test_trca_held_out_accuracy() -> None:
    train_x, train_y, test_x, test_y = _split_trca_dataset()
    classifier = TRCAClassifier(250.0, ridge=1e-4).fit(train_x, train_y)
    predictions = [classifier.predict(epoch).label_hz for epoch in test_x]
    accuracy = accuracy_score(test_y, predictions)
    assert accuracy >= 0.875, (test_y, predictions, accuracy)
    for decision in [classifier.predict(test_x[0]), classifier.predict(test_x[-1])]:
        assert 0.0 <= decision.best_score <= 1.0
        assert decision.margin >= 0.0


def test_online_window_cadence_and_label() -> None:
    fs = 250.0
    target = 12.0
    classifier = CCAClassifier([8.0, 10.0, 12.0, 15.0], fs, harmonics=3)
    stream = SSVEPStreamClassifier(
        classifier,
        fs,
        window_seconds=2.0,
        hop_seconds=0.5,
    )
    eeg = simulate_ssvep_window(
        target,
        fs,
        3.2,
        channels=4,
        signal_uv=14.0,
        noise_uv=2.0,
        seed=51,
    )

    decisions = []
    # Deliberately use irregular transport block sizes to verify sample-based
    # cadence does not depend on chunk boundaries.
    cursor = 0
    for block_size in [37, 81, 19, 133, 7, 211, 101, 211]:
        if cursor >= len(eeg):
            break
        block = eeg[cursor : min(cursor + block_size, len(eeg))]
        decisions.extend(stream.push(block))
        cursor += len(block)
    if cursor < len(eeg):
        decisions.extend(stream.push(eeg[cursor:]))

    expected_end_samples = [500, 625, 750]
    assert [d.end_sample for d in decisions] == expected_end_samples
    assert all(d.decision.label_hz == target for d in decisions)
    assert_close(decisions[0].end_time_s, 2.0, 1e-12)


def test_metrics() -> None:
    perfect_itr = itr_bits_per_minute(4, 1.0, 1.0)
    assert_close(perfect_itr, 120.0, 1e-12)
    assert itr_bits_per_minute(4, 0.25, 2.0) == 0.0

    truth = [8.0, 10.0, 12.0, 15.0]
    pred = [8.0, 10.0, None, 12.0]
    assert_close(accuracy_score(truth, pred), 0.5, 1e-12)
    cm = confusion_matrix(truth, pred, labels=truth)
    assert cm.shape == (4, 4)
    assert cm.sum() == 3  # rejected trial is intentionally absent from matrix.
    assert cm[0, 0] == 1 and cm[1, 1] == 1 and cm[3, 2] == 1


def main() -> None:
    tests = [
        test_harmonic_references,
        test_canonical_correlation_invariants,
        test_cca_multiple_targets,
        test_spectral_snr_prefers_target,
        test_fbcca_target_recovery,
        test_rejection_logic,
        test_trca_held_out_accuracy,
        test_online_window_cadence_and_label,
        test_metrics,
    ]
    print("ADS1299 SSVEP numerical regression tests")
    for test in tests:
        test()
        print(f"  PASS {test.__name__}")
    print(f"All {len(tests)} SSVEP tests passed.")


if __name__ == "__main__":
    main()
