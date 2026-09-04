import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "pc_tools" / "python"))

from packet import Frame, crc16_ccitt, decode_packet, encode_packet

GOLDEN_PACKET = bytes.fromhex(
    "a55a000340e2010006120f00c000000000000001000000ffffffffffff7f"
    "00000080ff7b00000038feffff2a0000007790"
)

f = Frame(
    version=0,
    flags=3,
    sequence=123456,
    timestamp_us=987654,
    status=b"\xC0\x00\x00",
    channels=(0, 1, -1, 8388607, -8388608, 123, -456, 42),
)

# CRC-16/IBM-3740 (CRC-16/CCITT-FALSE) canonical check value.
assert crc16_ccitt(b"123456789") == 0x29B1

# The Python implementation must match the same byte-exact vector used by the C test.
p = encode_packet(f)
assert p == GOLDEN_PACKET
assert len(p) == 49
assert decode_packet(GOLDEN_PACKET) == f
assert decode_packet(p) == f

bad_sync = bytearray(GOLDEN_PACKET)
bad_sync[0] ^= 0x01
try:
    decode_packet(bytes(bad_sync))
except ValueError as exc:
    assert "sync" in str(exc).lower()
else:
    raise AssertionError("bad sync was accepted")

bad_crc = bytearray(GOLDEN_PACKET)
bad_crc[20] ^= 0x01
try:
    decode_packet(bytes(bad_crc))
except ValueError as exc:
    assert "crc" in str(exc).lower()
else:
    raise AssertionError("bad CRC was accepted")

for malformed in (GOLDEN_PACKET[:-1], GOLDEN_PACKET + b"\x00"):
    try:
        decode_packet(malformed)
    except ValueError as exc:
        assert "49" in str(exc)
    else:
        raise AssertionError("bad packet length was accepted")

print("PC packet golden-vector and rejection tests passed")
