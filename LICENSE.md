# License / ライセンス

Copyright (C) 2023-2026 atsukita1969

This repository uses a different license for each kind of content. The licenses apply only to the author's own work; third-party material listed below is not covered.
このリポジトリは、内容の種類ごとに異なるライセンスを適用しています。ライセンスは作者自身の成果物にだけ適用され、下に挙げる他者の成果物は対象外です。

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

Purchased parts (motor, aluminium extrusions, urethane roller, bearings, hinges, switches, electronic components, modules and so on) are Available Components in the sense of CERN-OHL-S v2 section 1.7. They are identified by the part numbers in the bills of materials ([hardware/mechanical/bom.csv](hardware/mechanical/bom.csv), [hardware/pcb/bom.csv](hardware/pcb/bom.csv)).
購入部品（モーター、アルミフレーム、ウレタンローラー、ベアリング、ヒンジ、スイッチ、電子部品、モジュールなど）は、CERN-OHL-S v2 第1.7項の Available Component です。部品表の型番で特定しています。

## Third-party material / 他者の成果物

The following material is not covered by the licenses above. It remains under the terms of its owners.
次のものは上記のライセンスの対象外で、それぞれの権利者の条件に従います。

### Firmware and tools / ファームウェアとツール

Not included in this repository. / このリポジトリには含みません。

| Component | License |
|:--|:--|
| [TMCStepper](https://github.com/teemuatlut/TMCStepper) | MIT |
| [AccelStepper](https://www.airspayce.com/mikem/arduino/AccelStepper/) | GPL-3.0 (or a commercial license from its author) |
| Arduino AVR core, [ATTinyCore](https://github.com/SpenceKonde/ATTinyCore) | LGPL-2.1-or-later |
| [simavr](https://github.com/buserror/simavr) (used by `tools/sim`) | GPL-3.0 |

### PCB design files / 基板設計データ

The EAGLE files (`hardware/pcb/eagle/`) contain copies of library elements (symbols, footprints, package references) made by others. The author's own library is `my_mixed`.
EAGLE データには、他者が作成したライブラリ要素（回路図シンボル、フットプリント、パッケージ情報）の複製が含まれています。作者自身のライブラリは `my_mixed` です。

| Library in the EAGLE files | Source |
|:--|:--|
| `supply1`, `resistor`, `relay`, `transistor-neu-to92` | CadSoft / Autodesk, distributed with EAGLE |
| `Connector`, `Opto-Electronic`, `Power_Symbols` | Autodesk, distributed with EAGLE / Autodesk Fusion |
| `Diodes`, `Microcontroller` | PCBLayout.com |
| `Rembrandt Electronics - JST XH Connectors v1-0` | Rembrandt Electronics |
| `con-headers-jp` | Copyright (C) 2008 Bob Starr |
| `Seeed-IC_`, `arduinopromini`, `power-jack` | Third-party libraries; author and license are not stated in the files |
| `RDER73A103K2K1H03B` | Symbol and footprint of the Murata RDER73A103K2K1H03B capacitor from [SnapEDA (SnapMagic)](https://www.snapeda.com/), licensed under [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/) with the Design Exception 1.0 |

### Mechanical design files / 機構設計データ

`hardware/mechanical/step/assy_top_dual.stp` (complete assembly) contains 3D models of purchased parts and of the electronic components on the PCB, made by their manufacturers or other third parties. These models are included only to show the assembly and are not covered by the CERN-OHL-S. The parts designed by the author are also published separately in `hardware/mechanical/step/parts/`.
`assy_top_dual.stp`（全体組立）には、購入部品と基板上の電子部品の 3D モデル（メーカーまたは他者が作成したもの）が含まれています。これらは組立状態を示すためだけに含めており、CERN-OHL-S の対象外です。作者が設計した部品は `hardware/mechanical/step/parts/` にも個別に置いています。

## Note / 注意

The licenses cover the files in this repository (drawings, design data, source code, documents). They do not cover the underlying idea or mechanism.
ライセンスの対象は、このリポジトリのファイル（図面、設計データ、ソースコード、文書）です。仕組みやアイデアそのものは対象ではありません。
