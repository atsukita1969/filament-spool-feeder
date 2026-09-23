# Changelog / 変更履歴

## Unreleased (2026 brush-up)

### Firmware / ファームウェア

- Main controller rewritten as `firmware/feeder_main` (was `arduino/TMC2130_spool.ino`).
  - TMC2130Stepper (deprecated) replaced by TMCStepper 0.7.3. TMC2130 register values are unchanged.
  - Feed values kept. The real motion of the old firmware (about 2,390 steps/s while requested, jump to about 3,900 steps/s when the request ended) is reproduced with explicit values, without the speed jump.
  - Driver configuration is read back and verified; the driver is reconfigured automatically after a reset (e.g. motor supply interruption); overtemperature or short circuit latches a stop.
  - Feed request input filter, backup jam timeout (53 s), serial status messages, status LED on D8 (board rev.B).
- Jam monitor rewritten as `firmware/jam_monitor_tiny85` (was `arduino/motor_emergency_stop_tiny85.ino`).
  - Fixed: the timer was reset only when the input fell within a 10 ms window (about 1/3 of releases were missed).
  - Timeout measured in real time (48 s, the effective value of the old firmware at 8 MHz).
  - Serial removed (conflicted with the input pin on ATTinyCore), watchdog added, high-impedance output on fault, external sensor input for board rev.B.
- Verified in AVR simulation (`tools/sim`) against the old binaries. CI compiles all variants.

- メイン制御を `firmware/feeder_main` に書き直し。TMCStepper へ移行（TMC2130 の設定値は同一）。送り値は維持し、旧版の実際の動きを再現しつつ停止時の速度の跳ね上がりを除去。ドライバの設定確認と自動再設定、異常時の停止保持、入力フィルタ、予備のジャム判定、シリアル出力、状態 LED を追加。
- ジャム監視を `firmware/jam_monitor_tiny85` に書き直し。計時のリセット漏れを修正、実時間 48 秒で判定、Serial 削除、ウォッチドッグ、停止時のハイインピーダンス出力、rev.B の外部入力に対応。
- 旧版バイナリとの比較を AVR シミュレーションで実施（`tools/sim`）。CI で全構成をコンパイル。

### Hardware / ハードウェア

- EAGLE files (the data used for fabrication) added under `hardware/pcb/eagle`. DesignSpark files removed (GND pour disconnected in the conversion; available in tag `v1.0-2023`).
- BOMs converted to CSV and corrected: relay G6K-2P-Y DC5, PS1 RXEF135, T1 DTC144EKA, timing pulley added.
- Control board rev.B designed in Autodesk Fusion and exported as EAGLE 9.7.0 files (rev.A is in tag `pcb-rev.A`): external sensor input EXT_SENSE to ATtiny85 PB4 through R6 10 kΩ with pull-down R5 10 kΩ, T1 → DTC123JKA, R2 → 2.2 kΩ, D3 driven from Pro Mini D8 with R4 → 470 Ω, chip resistors 0603 → 0805. Connectivity checked on the Gerber output. Schematic PDF added.
- Complete assembly STEP (`assy_top_dual.stp`) removed because it contained manufacturer 3D models of purchased parts. Replaced by an outline drawing (`docs/images/assembly_outline.png`) and a list of the parts in the assembly model (`hardware/mechanical/assembly_parts.csv`). Motor mount added to the mechanical BOM.
- STEP files of the six self-made acrylic plates added to `hardware/mechanical/step/parts` (extracted unchanged from the assembly model). Mechanical BOM quantities corrected to the assembly model.

- 発注に使った EAGLE データを追加。DesignSpark データは削除（変換時に GND ベタが外れていたため。タグ `v1.0-2023` で参照可能）。
- 部品表を CSV にして修正（リレー DC5、PS1 RXEF135、T1 DTC144EKA、タイミングプーリー追加）。
- 制御基板 rev.B を Autodesk Fusion で作成し、EAGLE 9.7.0 形式で書き出し（rev.A はタグ `pcb-rev.A`）。外部センサー入力 EXT_SENSE（R6 10kΩ 経由で ATtiny85 PB4、R5 10kΩ でプルダウン）、T1 を DTC123JKA、R2 を 2.2kΩ、D3 を Pro Mini D8 で駆動（R4 470Ω）、チップ抵抗を 0603 から 0805 に変更。ガーバー出力で接続を確認。回路図 PDF を追加。
- 購入部品のメーカー製 3D モデルを含んでいた全体組立の STEP（`assy_top_dual.stp`）を削除し、組立外形図と、組立モデルの部品と数量の一覧に置き換え。機構の部品表にモーターマウントを追加。
- 自作のアクリル板6種の STEP を `hardware/mechanical/step/parts` に追加（組立モデルから形状を変えずに抽出）。機構の部品表の数量を組立モデルに合わせて修正。

### Repository / リポジトリ

- New layout (`firmware/`, `hardware/`, `docs/`, `tools/`), bilingual README, documentation of principle and wiring, licenses (CERN-OHL-S-2.0 / GPL-3.0-or-later / CC BY-SA 4.0), images stored in the repository.
- フォルダ構成を整理、README を日英併記、原理と配線の文書、ライセンス、画像をリポジトリ内に保存。

## v1.0-2023

- Original upload (April 2023). / 2023年4月の最初の公開。
