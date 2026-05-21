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

## Citation

N. Ahmed and M. I. Hussain,  
*"Implementation and Evaluation of a Tightly-Synchronized WiFi-based Long Distance Network for Rural Area Connectivity."*

```bibtex
@article{ahmed_wild_2c,
  author  = {Ahmed, Nurzaman and Hussain, Md. Iftekhar},
  title   = {Implementation and Evaluation of a Tightly-Synchronized WiFi-based Long Distance Network for Rural Area Connectivity},
  journal = {Technical Report},
  year    = {2024}
}
```

N. Ahmed, D. De and I. Hussain, "Internet of Things (IoT) for Smart Precision Agriculture and Farming in Rural Areas," in IEEE Internet of Things Journal, vol. 5, no. 6, pp. 4890-4899, Dec. 2018, doi: 10.1109/JIOT.2018.2879579.
keywords: {Agriculture;Smart devices;Internet of Things;Wireless sensor networks;Delays;Logic gates;Routing;Cloud computing;fog computing;Internet of Things (IoT);smart agriculture;WiFi-based long distance (WiLD) network},
BibTeX:

```bibtex
@ARTICLE{8521668,
  author={Ahmed, Nurzaman and De, Debashis and Hussain, Iftekhar},
  journal={IEEE Internet of Things Journal}, 
  title={Internet of Things (IoT) for Smart Precision Agriculture and Farming in Rural Areas}, 
  year={2018},
  volume={5},
  number={6},
  pages={4890-4899},
  keywords={Agriculture;Smart devices;Internet of Things;Wireless sensor networks;Delays;Logic gates;Routing;Cloud computing;fog computing;Internet of Things (IoT);smart agriculture;WiFi-based long distance (WiLD) network},
  doi={10.1109/JIOT.2018.2879579}}
```

S. S. Ahmed, I. Hussain and N. Ahmed, "Driver Level Implementation of TDMA MAC in Long Distance WiFi," 2015 International Conference on Computational Intelligence and Networks, Odisha, India, 2015, pp. 80-85, doi: 10.1109/CINE.2015.25.

BibTeX:

```bibtex
@inproceedings{ahmed2015driver,
  author    = {Ahmed, S. S. and Hussain, I. and Ahmed, N.},
  title     = {Driver Level Implementation of TDMA MAC in Long Distance WiFi},
  booktitle = {2015 International Conference on Computational Intelligence and Networks},
  year      = {2015},
  pages     = {80--85},
  address   = {Odisha, India},
  doi       = {10.1109/CINE.2015.25}
}
```
