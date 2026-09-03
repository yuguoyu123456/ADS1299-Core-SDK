"""Small fixed-size ring buffer for online EEG examples."""

from __future__ import annotations
import numpy as np


class RingBuffer:
    def __init__(self, samples: int, channels: int):
        if samples <= 0 or channels <= 0:
            raise ValueError("samples and channels must be positive")
        self.data = np.zeros((samples, channels), dtype=float)
        self.write_index = 0
        self.count = 0

    @property
    def capacity(self):
        return self.data.shape[0]

    def append(self, sample):
        sample = np.asarray(sample, dtype=float)
        if sample.shape != (self.data.shape[1],):
            raise ValueError("sample channel count mismatch")
        self.data[self.write_index] = sample
        self.write_index = (self.write_index + 1) % self.capacity
        self.count = min(self.count + 1, self.capacity)

    def latest(self, n=None):
        if n is None:
            n = self.count
        n = min(int(n), self.count)
        idx = (self.write_index - n + np.arange(n)) % self.capacity
        return self.data[idx].copy()
