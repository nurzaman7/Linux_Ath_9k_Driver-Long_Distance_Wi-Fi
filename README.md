# Linux Ath9k Long-Distance Wi-Fi Driver (2C MAC Research Port)

This repository packages source artifacts and operational scripts for a modified Atheros `ath9k/mac80211` stack used in long-distance Wi-Fi (WiLD) experiments with a TDMA-style 2C MAC approach.

It is now organized as a reproducible, portfolio-ready research codebase:
- Extracted source files are versioned under `src/`.
- Operational scripts are parameterized under `scripts/`.
- Original archives and papers are preserved under `archive/` and `docs/`.

## Repository Structure

- `src/driver_2c/`: extracted 2C-modified mac80211 source files (`main.c`, `rx.c`, `tx.c`, headers)
- `src/tools/sendraw/`: raw packet sender utility
- `src/tools/sendwlan/`: wlan packet sender utility
- `scripts/`: install/copy/configure helper scripts
- `docs/`: reference PDFs and project usage notes
- `archive/`: original compressed artifacts kept for provenance

## Quick Start

1. Build or obtain your target kernel modules (`*.ko`) for your OpenWrt/Linux target.
2. Copy modules into a local folder:

```bash
./scripts/copy_modules.sh <compat_wireless_build_root> ./build/modules
```

3. Install modules on target (requires root):

```bash
sudo ./scripts/install_modules.sh ./build/modules
```

4. Put interface into monitor mode (example):

```bash
sudo ./scripts/configure_monitor.sh wlan0 12 36
```

5. Build user-space packet tools:

```bash
make -C src/tools/sendraw
make -C src/tools/sendwlan
```

See detailed notes in [`docs/USAGE.md`](docs/USAGE.md).

## Compatibility Notes

- This code was originally developed around old `compat-wireless`/OpenWrt-era kernels.
- You should expect integration work for modern kernels.
- These scripts are intended for controlled research/testbed environments.

## Safety

- Module load/unload and monitor mode commands can interrupt connectivity.
- Run only on dedicated test devices.
- Validate local regulations and spectrum usage constraints before field experiments.

## Legacy References

Original research notes and manuals are in `docs/`. Original archives remain in `archive/`.
