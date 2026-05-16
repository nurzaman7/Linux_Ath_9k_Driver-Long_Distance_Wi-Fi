#!/usr/bin/env sh
set -eu

# Usage:
#   ./scripts/copy_modules.sh <openwrt_build_dir> [output_dir]
# Example:
#   ./scripts/copy_modules.sh ~/trunk/build_dir/.../compat-wireless-2014-01-23.1 ./build/modules

SRC_ROOT="${1:-}"
OUT_DIR="${2:-./build/modules}"

if [ -z "$SRC_ROOT" ]; then
  echo "Usage: $0 <openwrt_build_dir> [output_dir]" >&2
  exit 1
fi

mkdir -p "$OUT_DIR"

cp "$SRC_ROOT/net/mac80211/mac80211.ko" "$OUT_DIR/"
cp "$SRC_ROOT/net/wireless/cfg80211.ko" "$OUT_DIR/"
cp "$SRC_ROOT/drivers/net/wireless/ath/ath.ko" "$OUT_DIR/"
cp "$SRC_ROOT/drivers/net/wireless/ath/ath9k/ath9k.ko" "$OUT_DIR/"
cp "$SRC_ROOT/drivers/net/wireless/ath/ath9k/ath9k_hw.ko" "$OUT_DIR/"
cp "$SRC_ROOT/drivers/net/wireless/ath/ath9k/ath9k_common.ko" "$OUT_DIR/"
cp "$SRC_ROOT/compat/compat.ko" "$OUT_DIR/"

echo "Copied modules into: $OUT_DIR"
