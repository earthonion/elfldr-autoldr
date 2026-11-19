# elfldr-autoldr

PS5 elfldr with an `autoldr` extension. Fork of [ps5-payload-dev/elfldr](https://github.com/ps5-payload-dev/elfldr).

## What is this

`elfldr-ps5.elf` is the post-exploit ELF loader that PS5 jailbreak chains hand control to once they have kernel R/W. It listens on TCP 9020 (control) / 9021 (payload) and runs whatever ELF you send it.

This fork bundles two extras:

- **`autoldr.elf`** — chains off elfldr and automatically loads any `.elf` it finds in well-known paths (`/mnt/usb0..7/`, `/data/`, `/mnt/usbN/ps5_autoloader/`, `/data/ps5_autoloader/`). Drop your payload on a USB stick, plug it in, run elfldr — no `nc`/`socat` needed.
- An updated `Makefile` that builds everything in one shot and produces a single `elfldr-ps5.elf` artifact.

## Setup

Create an `autoload.txt` file listing the payloads to run, one per line. Place it and the `.elf` files together in one of these locations:

**USB stick** (checked first):
```
USB root/
├── autoload.txt
├── kstuff-lite.elf
└── ShadowMountPlus.elf
```

**Or in a subfolder:**
```
USB root/
└── ps5_autoloader/
    ├── autoload.txt
    ├── kstuff-lite.elf
    └── ShadowMountPlus.elf
```

**Internal storage** — same layout under `/data/` or `/data/ps5_autoloader/`.

Example `autoload.txt`:
```
kstuff-lite.elf
!3000
ShadowMountPlus.elf
```

Lines starting with `!` are delays in milliseconds — `!3000` waits 3 seconds before loading the next payload.

Autoldr checks paths in this order and **stops at the first `autoload.txt` it finds**:
1. `/mnt/usb0/` … `/mnt/usb7/`
2. `/data/`
3. `/mnt/usb0/ps5_autoloader/` … `/mnt/usb7/ps5_autoloader/`
4. `/data/ps5_autoloader/`

## Install

Replace `elfldr.elf` (or `elfldr-ps5.elf`) in your exploit with the build from [releases](https://git.etawen.dev/earthonion/elfldr-autoldr/releases).

| Exploit chain | File to replace |
|---|---|
| Netflix-N-Hack | `payloads/elfldr-ps5.elf` |
| Y2JB | `splash_screen/aHR0cHM6...vdHY=/elfldr-ps5-1340.elf` |

## Building

```
git clone https://git.etawen.dev/earthonion/elfldr-autoldr
cd elfldr-autoldr
export PS5_PAYLOAD_SDK=/opt/ps5-payload-sdk    # https://github.com/ps5-payload-dev/sdk
make
# -> elfldr-ps5.elf
```

## Credits

- [John Törnblom](https://github.com/john-tornblom) — original elfldr
- [ps5-payload-dev](https://github.com/ps5-payload-dev) — upstream fork + payload SDK
- `autoldr` extension by earthonion

## License

GPL-3.0 (same as upstream).
