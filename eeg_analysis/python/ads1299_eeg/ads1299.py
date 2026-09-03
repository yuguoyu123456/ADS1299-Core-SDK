"""ADS1299-specific host-side conversion helpers.

The ADS1299 outputs signed 24-bit two's-complement channel codes. According to
TI's data format, one LSB is ``(2 * VREF / gain) / 2**24`` volts referred to the
ADC input. These helpers keep that conversion explicit and testable.
"""

from __future__ import annotations

import numpy as np


ADS1299_BITS = 24
ADS1299_CODE_MIN = -(1 << 23)
ADS1299_CODE_MAX = (1 << 23) - 1


def sign_extend_24(value: int) -> int:
    """Convert an unsigned 24-bit word to a signed Python integer."""
    value = int(value)
    if value < 0 or value > 0xFFFFFF:
        raise ValueError("value must fit in 24 bits")
    return value - (1 << 24) if value & 0x800000 else value


def decode_24be(data: bytes | bytearray | memoryview) -> int:
    """Decode one ADS1299 channel value from three MSB-first bytes."""
    raw = bytes(data)
    if len(raw) != 3:
        raise ValueError("ADS1299 channel code requires exactly 3 bytes")
    return sign_extend_24((raw[0] << 16) | (raw[1] << 8) | raw[2])


def decode_channels_24be(payload: bytes | bytearray | memoryview, channels: int = 8) -> np.ndarray:
    """Decode consecutive 24-bit channel values from an ADS1299 payload."""
    raw = bytes(payload)
    if channels < 1:
        raise ValueError("channels must be positive")
    expected = channels * 3
    if len(raw) != expected:
        raise ValueError(f"expected {expected} channel bytes, got {len(raw)}")
    output = np.empty(channels, dtype=np.int32)
    for channel in range(channels):
        start = channel * 3
        output[channel] = decode_24be(raw[start : start + 3])
    return output


def lsb_volts(vref: float = 4.5, gain: float = 24.0) -> float:
    """Return ADS1299 input-referred volts per ADC code."""
    vref = float(vref)
    gain = float(gain)
    if not np.isfinite(vref) or vref <= 0:
        raise ValueError("vref must be positive and finite")
    if not np.isfinite(gain) or gain <= 0:
        raise ValueError("gain must be positive and finite")
    return (2.0 * vref / gain) / float(1 << ADS1299_BITS)


def codes_to_volts(codes: np.ndarray, vref: float = 4.5, gain: float = 24.0) -> np.ndarray:
    """Convert signed ADS1299 ADC codes to input-referred volts."""
    x = np.asarray(codes)
    if np.any(x < ADS1299_CODE_MIN) or np.any(x > ADS1299_CODE_MAX):
        raise ValueError("codes contain values outside signed 24-bit range")
    return x.astype(np.float64) * lsb_volts(vref=vref, gain=gain)


def codes_to_microvolts(codes: np.ndarray, vref: float = 4.5, gain: float = 24.0) -> np.ndarray:
    """Convert signed ADS1299 ADC codes to input-referred microvolts."""
    return codes_to_volts(codes, vref=vref, gain=gain) * 1e6


def volts_to_codes(volts: np.ndarray, vref: float = 4.5, gain: float = 24.0) -> np.ndarray:
    """Quantize input-referred volts into the ADS1299 signed-code range.

    This is useful for simulation and host-side round-trip tests. Values beyond
    full scale are clipped to the converter's output-code limits.
    """
    x = np.asarray(volts, dtype=float)
    codes = np.rint(x / lsb_volts(vref=vref, gain=gain))
    return np.clip(codes, ADS1299_CODE_MIN, ADS1299_CODE_MAX).astype(np.int32)
