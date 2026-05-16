# Usage Guide

## 1) Prepare Modules

Build your modified driver stack in your OpenWrt/Linux build tree, then copy modules:

```bash
./scripts/copy_modules.sh <compat_wireless_build_root> ./build/modules
```

Expected modules:
- `compat.ko`
- `cfg80211.ko`
- `mac80211.ko`
- `ath.ko`
- `ath9k_hw.ko`
- `ath9k_common.ko`
- `ath9k.ko`

## 2) Load Modules

```bash
sudo ./scripts/install_modules.sh ./build/modules
```

This unloads existing ath9k/mac80211 stack (best effort), then loads the copied modules in dependency order.

## 3) Configure Interface for Testbed Use

```bash
sudo ./scripts/configure_monitor.sh <iface> <host_id> <channel>
```

Example:

```bash
sudo ./scripts/configure_monitor.sh wlan0 12 36
```

The script configures:
- monitor mode
- static IP `10.42.43.<host_id>/24`
- requested channel

## 4) Build Packet Utilities

```bash
make -C src/tools/sendraw
make -C src/tools/sendwlan
```

Run examples (root typically required for raw sockets):

```bash
sudo ./src/tools/sendraw/sendraw wlan0
sudo ./src/tools/sendwlan/sendwlan wlan1
```

## 5) Source Location for 2C Logic

Primary extracted files:
- `src/driver_2c/main.c`
- `src/driver_2c/rx.c`
- `src/driver_2c/tx.c`
- `src/driver_2c/2C-main.h`
- `src/driver_2c/2C-rx.h`
- `src/driver_2c/2C-tx.h`

These are patch-style source artifacts and not a complete standalone kernel tree.
