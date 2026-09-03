import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "host" / "python"))

from packet import Frame, decode_packet, encode_packet

f = Frame(
    version=0,
    flags=3,
    sequence=123456,
    timestamp_us=987654,
    status=b"\xC0\x00\x00",
    channels=(0, 1, -1, 8388607, -8388608, 123, -456, 42),
)
p = encode_packet(f)
g = decode_packet(p)
assert g == f
assert len(p) == 49
print("host packet round-trip passed")
