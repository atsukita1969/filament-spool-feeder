# 配線とピン割り当て / Wiring and pin assignment

[日本語](#日本語) | [English](#english)

基板: `hardware/pcb/eagle/spool_tmc2130_b`（rev.B）。rev.A のデータはタグ `pcb-rev.A` にあります。部品番号は rev.A と rev.B で共通です（R6 は rev.B のみ）。
Board: `hardware/pcb/eagle/spool_tmc2130_b` (rev.B). The rev.A files are in tag `pcb-rev.A`. Reference designators are the same on rev.A and rev.B (R6 exists on rev.B only).

---

## 日本語

### コネクタ（すべて JST XH、X7 を除く）

| コネクタ | 用途 | ピン |
|:--|:--|:--|
| X1 | リミットスイッチ（送り要求） | 1 = D4（送り要求）、2 = +5V。スイッチで 1-2 を短絡すると送り要求 |
| X9 | センサー入力（X1 の代わり） | 1 = +5V、2 = GND、3 = D4 |
| X2 | ステッピングモーター | 1 = B−、2 = B+、3 = A−、4 = A+ |
| X3 | リセットスイッチ | 1 = GND、2 = RST（Pro Mini と ATtiny85 の両方） |
| X4 | 電源スイッチ | 1 = DC ジャック側（PS1 経由）、2 = +12V |
| X5 | 外部入力 | 1 = +5V、2 = EXT_IN（rev.A）。rev.B では 2 = EXT_SENSE（ATtiny85 PB4） |
| X6 | 5V 出力 | 1 = +5V、2 = GND |
| X7 | DC ジャック 2.1mm | 12V 入力、センタープラス |

### ジャンパー

| ジャンパー | 短絡するピン | 働き | 通常 |
|:--|:--|:--|:--|
| JP1 | 2-3 | リレーをバイパスし、モーター電源（VM）を常時供給。ジャム保護は働かない | 開放 |
| JP2 | 1-2 | rev.A: EXT_IN を +5V に固定（リレー常時 ON、ジャム保護は働かない）。rev.B: 外部センサーを使わない設定 | rev.A: 開放 / rev.B: センサーなしなら短絡 |

rev.A で ATtiny85 を実装したまま JP2 を短絡しないでください。2023 年版ファームでは、ジャム停止時に ATtiny85 の出力と +5V が短絡します。

rev.B の X5 には、正常なときに2番を HIGH にするセンサーをつなぎます（例：1-2 間の常閉接点）。詳しくは [hardware/pcb/README.md](../hardware/pcb/README.md) を参照してください。

### Arduino Pro Mini（5V、16MHz）

| ピン | 接続 |
|:--|:--|
| D4 | 送り要求（X1 / X9、10kΩ プルダウン R1、HIGH = 送り） |
| D5 | TMC2130 DIR |
| D6 | TMC2130 STEP |
| D7 | TMC2130 EN（LOW で有効） |
| D8 | 状態表示 LED D3（rev.B のみ） |
| D10 / D11 / D12 / D13 | TMC2130 CS / SDI(MOSI) / SDO(MISO) / SCK |
| RST | X3、ATtiny85 RESET と共通 |
| VCC | +5V（NJM7805 から） |

### ATtiny85

| ピン | 信号 | 接続 |
|:--|:--|:--|
| 1 | PB5 / RESET | RST（X3、Pro Mini と共通） |
| 2 | PB3 | EXT_IN → T1 → リレー K1（HIGH でモーター電源 ON、10kΩ プルダウン R3） |
| 3 | PB4 | rev.A: 未接続 / rev.B: EXT_SENSE（X5 から R6 10kΩ 経由。X5 側を R5 10kΩ でプルダウン） |
| 4 | GND | GND |
| 5 | PB0 | 未接続 |
| 6 | PB1 | D4（送り要求） |
| 7 | PB2 | 未接続 |
| 8 | VCC | +5V |

### TMC2130 モジュール（BIGTREETECH TMC2130、SPI モード）

| ヘッダ | ピン |
|:--|:--|
| J1（電源側） | 1 = GND、2 = VIO(+5V)、3 = A+、4 = A−、5 = B+、6 = B−、7 = GND、8 = VM（リレー経由の 12V） |
| J2（ロジック側） | 1 = EN、2 = SDI、3 = SCK、4 = CS、5 = SDO、6 = 未接続（CLK）、7 = STEP、8 = DIR |
| J3 | 未接続 |

### 電源の流れ

DC ジャック X7（12V）→ PS1（RXEF135）→ 電源スイッチ X4 → +12V → NJM7805 → +5V。+12V → リレー K1（NO 接点）→ VM（C1 100µF）→ TMC2130。

---

## English

### Connectors (all JST XH except X7)

| Connector | Use | Pins |
|:--|:--|:--|
| X1 | Limit switch (feed request) | 1 = D4 (feed request), 2 = +5V. Shorting 1-2 requests feed |
| X9 | Sensor input (alternative to X1) | 1 = +5V, 2 = GND, 3 = D4 |
| X2 | Stepper motor | 1 = B−, 2 = B+, 3 = A−, 4 = A+ |
| X3 | Reset switch | 1 = GND, 2 = RST (both Pro Mini and ATtiny85) |
| X4 | Power switch | 1 = from DC jack (via PS1), 2 = +12V |
| X5 | External input | 1 = +5V, 2 = EXT_IN (rev.A). On rev.B, 2 = EXT_SENSE (ATtiny85 PB4) |
| X6 | 5 V output | 1 = +5V, 2 = GND |
| X7 | DC jack 2.1 mm | 12 V input, center positive |

### Jumpers

| Jumper | Pins | Function | Normal |
|:--|:--|:--|:--|
| JP1 | 2-3 | Bypasses the relay; motor supply (VM) always on. No jam protection | Open |
| JP2 | 1-2 | rev.A: ties EXT_IN to +5V (relay always on, no jam protection). rev.B: external sensor not used | rev.A: open / rev.B: fitted when no sensor is used |

On rev.A, do not fit JP2 while the ATtiny85 is installed. With the 2023 firmware, the ATtiny85 output is shorted to +5V when it stops the motor on a jam.

On rev.B, connect a sensor to X5 that drives pin 2 HIGH when everything is OK (for example a normally closed contact between pins 1 and 2). See [hardware/pcb/README.md](../hardware/pcb/README.md).

### Arduino Pro Mini (5 V, 16 MHz)

| Pin | Connection |
|:--|:--|
| D4 | Feed request (X1 / X9, 10 kΩ pull-down R1, HIGH = feed) |
| D5 | TMC2130 DIR |
| D6 | TMC2130 STEP |
| D7 | TMC2130 EN (active LOW) |
| D8 | Status LED D3 (rev.B only) |
| D10 / D11 / D12 / D13 | TMC2130 CS / SDI (MOSI) / SDO (MISO) / SCK |
| RST | X3, shared with ATtiny85 RESET |
| VCC | +5V (from NJM7805) |

### ATtiny85

| Pin | Signal | Connection |
|:--|:--|:--|
| 1 | PB5 / RESET | RST (X3, shared with the Pro Mini) |
| 2 | PB3 | EXT_IN → T1 → relay K1 (HIGH = motor supply on, 10 kΩ pull-down R3) |
| 3 | PB4 | rev.A: not connected / rev.B: EXT_SENSE (from X5 through R6 10 kΩ, pulled down by R5 10 kΩ on the X5 side) |
| 4 | GND | GND |
| 5 | PB0 | not connected |
| 6 | PB1 | D4 (feed request) |
| 7 | PB2 | not connected |
| 8 | VCC | +5V |

### TMC2130 module (BIGTREETECH TMC2130, SPI mode)

| Header | Pins |
|:--|:--|
| J1 (power side) | 1 = GND, 2 = VIO (+5V), 3 = A+, 4 = A−, 5 = B+, 6 = B−, 7 = GND, 8 = VM (12 V through the relay) |
| J2 (logic side) | 1 = EN, 2 = SDI, 3 = SCK, 4 = CS, 5 = SDO, 6 = not connected (CLK), 7 = STEP, 8 = DIR |
| J3 | not connected |

### Power path

DC jack X7 (12 V) → PS1 (RXEF135) → power switch X4 → +12V → NJM7805 → +5V. +12V → relay K1 (NO contact) → VM (C1 100 µF) → TMC2130.
