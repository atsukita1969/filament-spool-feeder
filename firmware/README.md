# Firmware / ファームウェア

[日本語](#日本語) | [English](#english)

| Sketch | MCU | Role |
|:--|:--|:--|
| [feeder_main](feeder_main/) | Arduino Pro Mini (ATmega328P, 5 V, 16 MHz) | Feed control, TMC2130 driver, status LED |
| [jam_monitor_tiny85](jam_monitor_tiny85/) | ATtiny85 | Independent jam monitor, cuts the motor supply with the relay |

Both sketches are compiled by the CI ([.github/workflows/firmware.yml](../.github/workflows/firmware.yml)).

---

## 日本語

### 必要なもの

| 項目 | 内容 |
|:--|:--|
| Arduino IDE | 2.x（または arduino-cli） |
| ボード（メイン） | Arduino AVR Boards の「Arduino Pro or Pro Mini」、Processor「ATmega328P (5V, 16 MHz)」 |
| ボード（ATtiny85） | [ATTinyCore](https://github.com/SpenceKonde/ATTinyCore) 1.5.2 の「ATtiny25/45/85 (No bootloader)」。ボードマネージャ URL: `https://drazzy.com/package_drazzy.com_index.json` |
| ライブラリ | TMCStepper 0.7.3、AccelStepper 1.64（ライブラリマネージャから導入） |
| 書き込み器 | Pro Mini: USB シリアル変換（FTDI 6ピン）。ATtiny85: ISP 書き込み器（Arduino as ISP、USBasp など） |

旧版で使っていた TMC2130Stepper ライブラリは作者により非推奨となったため、後継の TMCStepper に移行しました。TMC2130 に書き込む設定値は旧版と同一です。

### Pro Mini への書き込み（feeder_main）

1. `feeder_main/feeder_main.ino` を開きます（`config.h` も同じフォルダに必要です）。
2. ボード「Arduino Pro or Pro Mini」、Processor「ATmega328P (5V, 16 MHz)」を選びます。
3. USB シリアル変換を Pro Mini の FTDI ヘッダにつなぎ、書き込みます。

### ATtiny85 への書き込み（jam_monitor_tiny85）

ATtiny85 は DIP ソケットに載っているので、基板から外して ISP 書き込み器で書き込みます。

1. ボード「ATtiny25/45/85 (No bootloader)」、Chip「ATtiny85」、Clock「8 MHz (internal)」、B.O.D.「B.O.D. Enabled (2.7v)」を選びます。
2. 書き込み器を選び、「ブートローダを書き込む」を実行します。ブートローダは書き込まれず、クロックと B.O.D. のヒューズだけが設定されます。新品の ATtiny85 は 1 MHz 設定なので、この操作をしないと時間が 8 倍に延びます。
3. 「書き込み装置を使って書き込む」でスケッチを書き込みます。
4. rev.B 基板（X5 を PB4 に接続した基板）では、`EXT_SENSE_ENABLED` を 1 にしてから書き込みます。

### 設定（feeder_main/config.h）

| 項目 | 既定値 | 内容 |
|:--|:--|:--|
| `FEED_STEPS` | 4000 | スイッチ OFF 後の送りステップ数 |
| `MOTION_MODE` | `MOTION_EXPLICIT` | 送りの動かし方。`MOTION_LEGACY` で 2023 年版と同じ呼び出し方になります |
| `EXPLICIT_MAX_SPEED` | 2400 | スイッチ ON 中の最高速度（ステップ/s） |
| `EXPLICIT_ACCEL_UP` | 4000 | ON 中の加速度（ステップ/s²） |
| `EXPLICIT_DECEL` | 2000 | OFF 後の減速度（ステップ/s²） |
| `RUN_CURRENT_MA` / `HOLD_MULT` | 600 / 0.5 | モーター電流（RMS）と保持電流の比 |
| `INPUT_FILTER_MS` | 5 | 送り要求入力のフィルタ時間 |
| `DRIVER_CHECK_MS` | 500 | TMC2130 の状態確認の周期 |
| `BACKUP_TIMEOUT_MS` | 53000 | ソフトウェア側のジャム判定（0 で無効） |

各値の根拠は [docs/principle.md](../docs/principle.md) にあります。

### 設定（jam_monitor_tiny85.ino の先頭）

| 項目 | 既定値 | 内容 |
|:--|:--|:--|
| `JAM_TIMEOUT_MS` | 48000 | 送り要求がこの時間続いたらモーター電源を切る |
| `RELEASE_FILTER_MS` | 20 | これより短い OFF では計時をリセットしない |
| `EXT_SENSE_ENABLED` | 0 | 1 = rev.B 基板の外部入力（PB4）を使う |
| `EXT_FAULT_FILTER_MS` | 50 | 外部入力がこの時間 LOW で異常と判定 |
| `EXT_STARTUP_GRACE_MS` | 1000 | 電源投入後、外部入力が正常になるまで待つ時間 |

### シリアル出力（Pro Mini、115200 bps）

| メッセージ | 意味 |
|:--|:--|
| `feeder_main start` | 起動 |
| `TMC2130: configured and verified` | ドライバ設定完了 |
| `TMC2130: reset / undervoltage detected (motor supply interrupted?)` | モーター電源の瞬断などでドライバがリセットされた（自動で再設定） |
| `TMC2130: not responding` | ドライバが応答しない（モーター電源が切れている、など） |
| `FAULT: TMC2130 overtemperature or short circuit. Latched until reset.` | 過熱・短絡で停止 |
| `FAULT: feed request active too long (jam?). Latched until reset.` | ソフトウェア側のジャム判定で停止 |

### 検証

旧版と新版のバイナリを AVR シミュレーターで動かし、送りの動き、TMC2130 のレジスタ値、保護機能を比較しました。手順と結果は [tools/sim](../tools/sim/) にあります。

---

## English

### Requirements

| Item | Details |
|:--|:--|
| Arduino IDE | 2.x (or arduino-cli) |
| Board (main) | Arduino AVR Boards, "Arduino Pro or Pro Mini", Processor "ATmega328P (5V, 16 MHz)" |
| Board (ATtiny85) | [ATTinyCore](https://github.com/SpenceKonde/ATTinyCore) 1.5.2, "ATtiny25/45/85 (No bootloader)". Boards Manager URL: `https://drazzy.com/package_drazzy.com_index.json` |
| Libraries | TMCStepper 0.7.3, AccelStepper 1.64 (Library Manager) |
| Programmer | Pro Mini: USB-serial adapter (6-pin FTDI). ATtiny85: ISP programmer (Arduino as ISP, USBasp, ...) |

The TMC2130Stepper library used by the old version has been deprecated by its author, so the firmware now uses its successor TMCStepper. The values written to the TMC2130 are identical to the old version.

### Flashing the Pro Mini (feeder_main)

1. Open `feeder_main/feeder_main.ino` (`config.h` must be in the same folder).
2. Select "Arduino Pro or Pro Mini", Processor "ATmega328P (5V, 16 MHz)".
3. Connect a USB-serial adapter to the FTDI header of the Pro Mini and upload.

### Flashing the ATtiny85 (jam_monitor_tiny85)

The ATtiny85 sits in a DIP socket. Take it out and program it with an ISP programmer.

1. Select "ATtiny25/45/85 (No bootloader)", Chip "ATtiny85", Clock "8 MHz (internal)", B.O.D. "B.O.D. Enabled (2.7v)".
2. Select the programmer and run "Burn Bootloader". No bootloader is written; this only sets the clock and B.O.D. fuses. A new ATtiny85 runs at 1 MHz, so without this step all times become 8 times longer.
3. Upload with "Upload Using Programmer".
4. For board rev.B (X5 wired to PB4), set `EXT_SENSE_ENABLED` to 1 before uploading.

### Settings (feeder_main/config.h)

| Item | Default | Meaning |
|:--|:--|:--|
| `FEED_STEPS` | 4000 | Steps fed after the switch opens |
| `MOTION_MODE` | `MOTION_EXPLICIT` | Motion profile. `MOTION_LEGACY` uses the same call pattern as the 2023 firmware |
| `EXPLICIT_MAX_SPEED` | 2400 | Top speed while the switch is on (steps/s) |
| `EXPLICIT_ACCEL_UP` | 4000 | Acceleration while the switch is on (steps/s²) |
| `EXPLICIT_DECEL` | 2000 | Deceleration after the switch opens (steps/s²) |
| `RUN_CURRENT_MA` / `HOLD_MULT` | 600 / 0.5 | Motor current (RMS) and hold current ratio |
| `INPUT_FILTER_MS` | 5 | Filter time of the feed request input |
| `DRIVER_CHECK_MS` | 500 | TMC2130 check period |
| `BACKUP_TIMEOUT_MS` | 53000 | Software jam timeout (0 = off) |

The reasoning behind the values is in [docs/principle.md](../docs/principle.md).

### Settings (top of jam_monitor_tiny85.ino)

| Item | Default | Meaning |
|:--|:--|:--|
| `JAM_TIMEOUT_MS` | 48000 | Cut the motor supply when the feed request lasts this long |
| `RELEASE_FILTER_MS` | 20 | Shorter off periods do not reset the timer |
| `EXT_SENSE_ENABLED` | 0 | 1 = use the external input (PB4) of board rev.B |
| `EXT_FAULT_FILTER_MS` | 50 | External input LOW this long = fault |
| `EXT_STARTUP_GRACE_MS` | 1000 | Time after power-up for the external input to become OK |

### Serial output (Pro Mini, 115200 bps)

| Message | Meaning |
|:--|:--|
| `feeder_main start` | Start-up |
| `TMC2130: configured and verified` | Driver configured |
| `TMC2130: reset / undervoltage detected (motor supply interrupted?)` | Driver was reset, e.g. by a short motor supply interruption (reconfigured automatically) |
| `TMC2130: not responding` | Driver does not respond (e.g. motor supply cut) |
| `FAULT: TMC2130 overtemperature or short circuit. Latched until reset.` | Stopped on overtemperature or short circuit |
| `FAULT: feed request active too long (jam?). Latched until reset.` | Stopped by the software jam timeout |

### Verification

The old and new binaries were run in an AVR simulator to compare motion, TMC2130 register values and protection functions. See [tools/sim](../tools/sim/).
