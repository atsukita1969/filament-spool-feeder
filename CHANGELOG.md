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
- rev.B change plan: external input to ATtiny85 PB4, T1 → DTC123JKA, R2 → 2.2 kΩ, D3 driven from Pro Mini D8 with R4 → 470 Ω.

- 発注に使った EAGLE データを追加。DesignSpark データは削除（変換時に GND ベタが外れていたため。タグ `v1.0-2023` で参照可能）。
- 部品表を CSV にして修正（リレー DC5、PS1 RXEF135、T1 DTC144EKA、タイミングプーリー追加）。
- rev.B の変更案を追加。

### Repository / リポジトリ

- New layout (`firmware/`, `hardware/`, `docs/`, `tools/`), bilingual README, documentation of principle and wiring, licenses (CERN-OHL-S-2.0 / GPL-3.0-or-later / CC BY-SA 4.0), images stored in the repository.
- フォルダ構成を整理、README を日英併記、原理と配線の文書、ライセンス、画像をリポジトリ内に保存。

## v1.0-2023

- Original upload (April 2023). / 2023年4月の最初の公開。
