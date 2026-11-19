# elfldr-autoldr

PS5 elfldr with an `autoldr` extension. Fork of [ps5-payload-dev/elfldr](https://github.com/ps5-payload-dev/elfldr).

## What is this

`elfldr-ps5.elf` is the post-exploit ELF loader that PS5 jailbreak chains hand control to once they have kernel R/W. It listens on TCP 9020 (control) / 9021 (payload) and runs whatever ELF you send it.

This fork bundles two extras:

- **`autoldr.elf`** — chains off elfldr and automatically loads any `.elf` it finds in well-known paths (`/mnt/usb0..7/`, `/data/`, `/mnt/usbN/ps5_autoloader/`, `/data/ps5_autoloader/`). Drop your payload on a USB stick, plug it in, run elfldr — no `nc`/`socat` needed.
- An updated `Makefile` that builds everything in one shot and produces a single `elfldr-ps5.elf` artifact.

## Usage

It's as simple as **replacing `elfldr.elf` (or `elfldr-ps5.elf`) in your favorite exploit** with the build from [releases](https://git.etawen.dev/earthonion/elfldr-autoldr/releases).

| Exploit chain | File to replace |
|---|---|
| Netflix-N-Hack | `payloads/elfldr-ps5.elf` |
| Y2JB | `splash_screen/aHR0cHM6...vdHY=/elfldr-ps5-1340.elf` |

> ⚠️ **Bump the download buffer.** The autoldr build is bigger than stock elfldr — current release is **~520 KB** (vs ~360 KB upstream). Most exploit chains pre-allocate a fixed buffer to fetch the ELF into; if that buffer is smaller than the new file, fetch silently truncates and the loader crashes on entry. Grep your chain for the elfldr `malloc(...)` / `fetch_file(..., SIZE)` call and bump it to at least `1 * 1024 * 1024` (1 MB) to give headroom for future builds.
>
> Known spots:
> - Netflix-N-Hack `payloads/elf_loader.js` — `malloc(...)` + `fetch_file(..., SIZE)` near the top
> - Netflix-N-Hack `payloads/p2jb.js` — same pattern
> - Y2JB `splash_screen/.../elf_loader.js` — same pattern

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
