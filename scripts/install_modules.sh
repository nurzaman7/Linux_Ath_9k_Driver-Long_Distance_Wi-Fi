#!/usr/bin/env sh
set -eu

# Usage:
#   sudo ./scripts/install_modules.sh /path/to/module-dir
# Example:
#   sudo ./scripts/install_modules.sh ./build/modules

MODULE_DIR="${1:-.}"

require_file() {
  if [ ! -f "$1" ]; then
    echo "Missing module: $1" >&2
    exit 1
  fi
}

for mod in compat cfg80211 mac80211 ath ath9k_hw ath9k_common ath9k; do
  require_file "$MODULE_DIR/$mod.ko"
done

for mod in ath9k ath9k_common ath9k_hw ath mac80211 cfg80211 compat; do
  rmmod "$mod" 2>/dev/null || true
done

insmod "$MODULE_DIR/compat.ko"
insmod "$MODULE_DIR/cfg80211.ko"
insmod "$MODULE_DIR/mac80211.ko"
insmod "$MODULE_DIR/ath.ko"
insmod "$MODULE_DIR/ath9k_hw.ko"
insmod "$MODULE_DIR/ath9k_common.ko"
insmod "$MODULE_DIR/ath9k.ko"

echo "Driver modules loaded from: $MODULE_DIR"
