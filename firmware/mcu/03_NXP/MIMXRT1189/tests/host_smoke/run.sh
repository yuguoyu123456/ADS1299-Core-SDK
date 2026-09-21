#!/usr/bin/env sh
set -eu

# Portable host-only compile smoke for the MIMXRT1189 ADS1299 leaf.
# This does not claim MCUXpresso, target linking, flashing, or board validation.
SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
BUILD_DIR="${1:-$SCRIPT_DIR/build}"

cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug
cmake --build "$BUILD_DIR" --target rt1189_host_smoke --parallel

echo "RT1189 host compile smoke: PASS"
echo "Validation scope: host C/API/include compilation only; NOT MCUXpresso/board verified."
