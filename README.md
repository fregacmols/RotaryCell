# RotaryCell

RotaryCell is a personal development project that converts a Western Electric/AT&T Model 500 rotary telephone into a self-contained cellular telephone while preserving the original handset, rotary dial, ringer, and familiar call-progress sounds.

This repository is a working engineering archive, not a release. The present prototype operates, but the hardware files and older documentation have not yet been consolidated into a repeatable public build. Treat anything under `hardware/experimental` or `docs/legacy` as reference material rather than construction instructions.

## Current status

- Current firmware: **v0.10.4**
- Controller/modem: LilyGO T-A7670G S3 (R2/Standard configuration)
- Subscriber-line interface: Silvertel Ag1171
- Power: Samsung 58E 21700 cell in the current prototype
- Telephone functions: rotary pulse dialing, incoming and outgoing cellular calls, physical bell ringing, dial tone, reorder tone, and receiver-off-hook warning tone
- Maintenance: temporary Wi-Fi dashboard and console, persistent event log, cellular-network clock, diagnostics, and application OTA update

The current firmware waits three seconds after the final dialed digit. Numbers shorter than seven digits and the configured blocked-number classes receive reorder treatment. An untouched off-hook line progresses from dial tone to reorder and then to the receiver-off-hook warning.

## Repository layout

| Path | Contents |
| --- | --- |
| `firmware/current` | Current Arduino sketch and source files |
| `firmware/prebuilt` | Current application OTA binary and source ZIP |
| `firmware/archive` | Historical firmware snapshots |
| `hardware/prototype` | Material associated with the working prototype |
| `hardware/experimental` | Unfinalized schematics, layouts, libraries, and alternatives |
| `hardware/legacy` | Older hardware documentation retained for reference |
| `docs/legacy` | Older version-specific documentation |
| `site` | Draft project-page copy for evilroot.net |

Start with [`BUILDING.md`](BUILDING.md) and the README inside the current sketch folder.

## Important cautions

- Do not connect prototype Tip/Ring wiring or a repurposed telephone jack to the public telephone network or house telephone wiring.
- The proposed rear-jack charging arrangement is still a prototype feature. Clearly label it and verify polarity and protection before use.
- Lithium-ion cells require suitable protection, charging, fusing, insulation, and mechanical restraint.
- The maintenance access point currently uses the development password `rotarycell`. Change `WIFI_AP_PASSWORD` in `Config.h` before using the phone where untrusted people could be nearby.

## Repository policy

For now, this repository is best kept **private** on GitHub. That provides remote access without presenting the project as a finished release. A page placed on evilroot.net is public even if it is not advertised; the draft in `site/` is intentionally modest and contains no download claim.

## License

Code and original documentation in this repository are licensed under the MIT License. Third-party datasheets, vendor names, trademarks, and historical telephone designs remain the property of their respective owners.

