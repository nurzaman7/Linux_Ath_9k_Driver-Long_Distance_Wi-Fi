#!/usr/bin/env sh
set -eu

# Usage:
#   sudo ./scripts/configure_monitor.sh <iface> <host_id> <channel>
# Example:
#   sudo ./scripts/configure_monitor.sh wlan0 12 36

IFACE="${1:-}"
HOST_ID="${2:-}"
CHANNEL="${3:-}"

if [ -z "$IFACE" ] || [ -z "$HOST_ID" ] || [ -z "$CHANNEL" ]; then
  echo "Usage: $0 <iface> <host_id> <channel>" >&2
  exit 1
fi

ifconfig "$IFACE" down
iwconfig "$IFACE" mode monitor
ifconfig "$IFACE" "10.42.43.$HOST_ID" netmask 255.255.255.0 up
iwconfig "$IFACE" channel "$CHANNEL"

echo "Configured $IFACE in monitor mode on channel $CHANNEL"
