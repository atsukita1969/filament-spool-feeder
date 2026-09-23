# Control board / 制御基板

[日本語](#日本語) | [English](#english)

![PCB render](../../docs/images/pcb_render.jpg)

---

## 日本語

### ファイル

| ファイル | 内容 |
|:--|:--|
| [eagle/spool_tmc2130_b.sch](eagle/spool_tmc2130_b.sch) | 回路図（EAGLE 9.6.2、XML） |
| [eagle/spool_tmc2130_b.brd](eagle/spool_tmc2130_b.brd) | 基板（EAGLE 9.6.2、XML、2層、50 × 90 mm）。発注に使ったデータ |
| [bom.csv](bom.csv) | 部品表（rev.A の実装品と rev.B の変更予定） |

EAGLE データは Autodesk Fusion の Electronics で開けます。KiCad でも取り込めます。配線とピン割り当ては [docs/wiring.md](../../docs/wiring.md) にあります。

### rev.A（製作済み）の注意点

- GND は配線ではなく表裏のベタだけで接続しています。
- リレー K1 のコイルは 5V 駆動です。部品は G6K-2P-Y **DC5** を使ってください（DC12 品は 5V では動作しません）。
- T1（DTC144EKA）はデータシート上の駆動能力に余裕がありません。実機では動いていますが、rev.B で変更します。
- R2（680Ω、0603）は 12V で約 0.15W 消費し、一般的な 0603 抵抗の定格 0.1W を超えます。
- ATtiny85 を実装したまま JP2 を短絡しないでください。
- 以前公開していた DesignSpark 版のデータは、EAGLE からの変換時に GND ベタが GND ネットから外れていたため削除しました。タグ `v1.0-2023` から参照できます。

### rev.B（改版予定）の変更点

| # | 変更 | 理由 |
|:--|:--|:--|
| 1 | X5 の2番と JP2 の1番を EXT_IN から外し、新しいネット EXT_SENSE として ATtiny85 PB4（3番ピン）へ接続。R5（10kΩ）でプルダウン | 外部センサーの異常でモーター電源を切れるようにするため（rev.A では ATtiny85 の出力と衝突して機能しない）。HIGH = 正常、LOW または断線 = 異常。JP2 の1-2 短絡で「外部センサーなし」 |
| 2 | T1 を DTC144EKA から DTC123JKA に変更（同じ SMT3、同じ端子配置） | 入力電流が 68〜126µA から 1.45〜2.69mA に増え、コイル電流 21.1mA で確実に飽和する |
| 3 | R2 を 680Ω から 2.2kΩ に変更 | 抵抗の損失を約 0.045W に下げる |
| 4 | D3 を D4 ネットから外し、Pro Mini D8 → R4 → D3 → GND で駆動。R4 を 150Ω から 470Ω に変更 | X9 にどの種類のセンサーでも接続できるようにし、LED で状態を表示するため。D8 の電流は約 6mA |

ファームは rev.A と rev.B の両方に対応しています（[firmware/](../../firmware/)）。

---

## English

### Files

| File | Contents |
|:--|:--|
| [eagle/spool_tmc2130_b.sch](eagle/spool_tmc2130_b.sch) | Schematic (EAGLE 9.6.2, XML) |
| [eagle/spool_tmc2130_b.brd](eagle/spool_tmc2130_b.brd) | Board (EAGLE 9.6.2, XML, 2 layers, 50 × 90 mm). The data used for fabrication |
| [bom.csv](bom.csv) | Bill of materials (rev.A as built and planned rev.B changes) |

The EAGLE files open in Autodesk Fusion Electronics and can be imported into KiCad. Wiring and pin assignment are in [docs/wiring.md](../../docs/wiring.md).

### Notes on rev.A (built)

- GND is connected only through the top and bottom copper pours, not by traces.
- The coil of relay K1 is driven from 5 V. Use G6K-2P-Y **DC5** (the DC12 version does not operate on 5 V).
- T1 (DTC144EKA) has no guaranteed drive margin according to its datasheet. It works on the built boards but is changed in rev.B.
- R2 (680 Ω, 0603) dissipates about 0.15 W at 12 V, above the 0.1 W rating of a standard 0603 resistor.
- Do not fit JP2 while the ATtiny85 is installed.
- The DesignSpark files published earlier were removed because the GND pour lost its connection to the GND net in the conversion from EAGLE. They are still available in tag `v1.0-2023`.

### Planned changes for rev.B

| # | Change | Reason |
|:--|:--|:--|
| 1 | Disconnect X5 pin 2 and JP2 pin 1 from EXT_IN and route them as a new net EXT_SENSE to ATtiny85 PB4 (pin 3), with a 10 kΩ pull-down R5 | Lets an external sensor cut the motor supply (on rev.A it conflicts with the ATtiny85 output and does not work). HIGH = OK, LOW or broken wire = fault. JP2 1-2 fitted = no external sensor |
| 2 | T1: DTC144EKA → DTC123JKA (same SMT3 package and pinout) | Input current rises from 68–126 µA to 1.45–2.69 mA, so the transistor saturates reliably at the 21.1 mA coil current |
| 3 | R2: 680 Ω → 2.2 kΩ | Reduces the resistor dissipation to about 0.045 W |
| 4 | Disconnect D3 from the D4 net and drive it from Pro Mini D8 → R4 → D3 → GND; R4: 150 Ω → 470 Ω | Any type of sensor can then be used on X9, and the LED shows the status. D8 current about 6 mA |

The firmware supports both rev.A and rev.B ([firmware/](../../firmware/)).
