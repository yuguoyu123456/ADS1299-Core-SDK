"""ADS1299-Core-SDK SSVEP BCI toolkit."""

from .cca import CCAClassifier, canonical_correlation, cca_scores
from .core import SSVEPDecision, decision_from_scores
from .fbcca import FBCCAClassifier, default_subbands, fbcca_scores, filter_bank_weights
from .metrics import accuracy_score, confusion_matrix, itr_bits_per_minute
from .online import OnlineSSVEPDecision, SSVEPStreamClassifier
from .references import harmonic_references
from .spectral import frequency_snr_db, harmonic_power_score, spectral_scores
from .synthetic import simulate_ssvep_dataset, simulate_ssvep_window
from .trca import TRCAClassifier, trca_spatial_filter

__all__ = [
    "CCAClassifier",
    "FBCCAClassifier",
    "TRCAClassifier",
    "SSVEPDecision",
    "OnlineSSVEPDecision",
    "SSVEPStreamClassifier",
    "canonical_correlation",
    "cca_scores",
    "fbcca_scores",
    "trca_spatial_filter",
    "harmonic_references",
    "default_subbands",
    "filter_bank_weights",
    "frequency_snr_db",
    "harmonic_power_score",
    "spectral_scores",
    "decision_from_scores",
    "simulate_ssvep_window",
    "simulate_ssvep_dataset",
    "accuracy_score",
    "confusion_matrix",
    "itr_bits_per_minute",
]

__version__ = "0.1.0"
