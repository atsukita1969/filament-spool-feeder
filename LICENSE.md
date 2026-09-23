# License / ライセンス

Copyright (C) 2023-2026 atsukita1969

This repository uses a different license for each kind of content. The licenses apply only to the author's own work; third-party material listed below is not covered.
このリポジトリは、内容の種類ごとに異なるライセンスを適用しています。ライセンスは作者自身の成果物にだけ適用され、下に挙げる他者の成果物は対象外です。

| Scope / 対象 | License / ライセンス | Text / 本文 |
|:--|:--|:--|
| `hardware/` — PCB design files (EAGLE) and schematic PDF, mechanical design files (STEP), bills of materials / 基板設計データと回路図 PDF、機構設計データ、部品表 | CERN Open Hardware Licence Version 2 - Strongly Reciprocal (`CERN-OHL-S-2.0`) | [LICENSES/CERN-OHL-S-2.0.txt](LICENSES/CERN-OHL-S-2.0.txt) |
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

The EAGLE files (`hardware/pcb/eagle/`) contain copies of library elements (symbols, footprints, package references) made by others, and the schematic PDF shows those symbols. The author's own library is `my_mixed`.
EAGLE データには、他者が作成したライブラリ要素（回路図シンボル、フットプリント、パッケージ情報）の複製が含まれ、回路図 PDF にはそのシンボルが描かれています。作者自身のライブラリは `my_mixed` です。

| Library in the EAGLE files | Source |
|:--|:--|
| `supply1`, `resistor`, `relay`, `transistor-neu-to92` | CadSoft / Autodesk, distributed with EAGLE |
| `Connector`, `Opto-Electronic`, `Power_Symbols`, `Resistor` | Autodesk, distributed with EAGLE / Autodesk Fusion |
| `Diodes`, `Microcontroller` | PCBLayout.com |
| `Rembrandt Electronics - JST XH Connectors v1-0` | Rembrandt Electronics |
| `con-headers-jp` | Copyright (C) 2008 Bob Starr |
| `Seeed-IC_`, `arduinopromini`, `power-jack` | Third-party libraries; author and license are not stated in the files |
| `RDER73A103K2K1H03B` | Symbol and footprint of the Murata RDER73A103K2K1H03B capacitor from [SnapEDA (SnapMagic)](https://www.snapeda.com/), licensed under [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/) with the Design Exception 1.0 |

### Mechanical design files / 機構設計データ

The STEP files in `hardware/mechanical/step/parts/` contain only parts designed by the author. The complete assembly STEP (`assy_top_dual.stp`), which contained 3D models of purchased parts made by their manufacturers, was removed in 2026 and is not covered by the CERN-OHL-S. The outline drawing (`docs/images/assembly_outline.png`) and the rendering (`docs/images/feeder_assembly_render.jpg`) show purchased parts only as pictures of the assembly.
`hardware/mechanical/step/parts/` の STEP は作者が設計した部品だけです。購入部品のメーカー製 3D モデルを含んでいた全体組立の STEP（`assy_top_dual.stp`）は 2026年に削除しました。これは CERN-OHL-S の対象外です。組立外形図とレンダリング画像では、購入部品は組立状態を示す絵としてだけ描かれています。

## Note / 注意

The licenses cover the files in this repository (drawings, design data, source code, documents). They do not cover the underlying idea or mechanism.
ライセンスの対象は、このリポジトリのファイル（図面、設計データ、ソースコード、文書）です。仕組みやアイデアそのものは対象ではありません。
