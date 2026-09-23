# License / ライセンス

Copyright (C) 2023-2026 atsukita1969

This repository uses a different license for each kind of content.
このリポジトリは、内容の種類ごとに異なるライセンスを適用しています。

| Scope / 対象 | License / ライセンス | Text / 本文 |
|:--|:--|:--|
| `hardware/` — PCB design files (EAGLE), mechanical design files (STEP), bills of materials / 基板設計データ、機構設計データ、部品表 | CERN Open Hardware Licence Version 2 - Strongly Reciprocal (`CERN-OHL-S-2.0`) | [LICENSES/CERN-OHL-S-2.0.txt](LICENSES/CERN-OHL-S-2.0.txt) |
| `firmware/`, `tools/`, `.github/` — firmware and tools / ファームウェアとツール | GNU General Public License v3.0 or later (`GPL-3.0-or-later`) | [LICENSES/GPL-3.0.txt](LICENSES/GPL-3.0.txt) |
| `docs/`, `README.md`, `CHANGELOG.md`, `LICENSE.md` and all other documentation and images / 文書と画像 | Creative Commons Attribution-ShareAlike 4.0 International (`CC-BY-SA-4.0`) | [LICENSES/CC-BY-SA-4.0.txt](LICENSES/CC-BY-SA-4.0.txt) |

## Hardware notice (CERN-OHL-S-2.0)

Copyright atsukita1969 2023-2026.
This source describes Open Hardware and is licensed under the CERN-OHL-S v2.

You may redistribute and modify this source and make products using it under the terms of the CERN-OHL-S v2 (https://ohwr.org/cern_ohl_s_v2.txt).

This source is distributed WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING OF MERCHANTABILITY, SATISFACTORY QUALITY AND FITNESS FOR A PARTICULAR PURPOSE. Please see the CERN-OHL-S v2 for applicable conditions.

Source location: https://github.com/atsukita1969/filament-spool-feeder

As per CERN-OHL-S v2 section 4, should You produce hardware based on this source, You must where practicable maintain the Source Location visible on the external case of the filament spool feeder or other products you make using this source.

## Third-party components / 他者の成果物

- The firmware uses the libraries [TMCStepper](https://github.com/teemuatlut/TMCStepper) (MIT) and [AccelStepper](https://www.airspayce.com/mikem/arduino/AccelStepper/) (GPL-3.0), which are not included in this repository.
- The PCB design uses parts from EAGLE libraries (e.g. Arduino Pro Mini footprint, JST XH connectors by Rembrandt Electronics, SnapEDA parts). Their licenses remain with their authors.

## Note / 注意

The licenses cover the files in this repository (drawings, design data, source code, documents). They do not cover the underlying idea or mechanism.
ライセンスの対象は、このリポジトリのファイル（図面、設計データ、ソースコード、文書）です。仕組みやアイデアそのものは対象ではありません。
