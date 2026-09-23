# 原理と制御 / Principle and control

[日本語](#日本語) | [English](#english)

---

## 日本語

### 1. 解決したい問題

ダイレクト式のプリンタでは、エクストルーダーがスプールからフィラメントを引き出します。スプールの慣性と回転抵抗は、そのままエクストルーダーの負荷になります。

最も大きい負荷は、ノズルがテーブル上でXY方向に大きく移動したときの引っ張りです。移動した距離の分だけフィラメントが引っ張られるため、消費量とは関係なく供給側に負荷がかかります。

### 2. 構成

```
ステッピングモーター 17HS08-1004S (1.8°)
  └ GT2 20歯プーリー ─ GT2無端ベルト 188mm ─ ウレタンローラー φ30（プーリーなし、ベルトを直接巻き掛け）
       └ スプールの縁（外周）がローラーに載り、摩擦で回る
            └ フィラメント ─ ゴム紐付きレバー（スイッチ） ─ プリンタ
```

### 3. 送り出しの制御

- ゴム紐付きのレバーが一定以上引っ張られるとスイッチが入り、モーターが回ってスプールから送り出します。
- スイッチが切れた後も 4000 ステップ送ってから止まります。
- フィラメントは使用量より多めに送り出されます。ゴム紐の伸びとフィラメントのたるみで 200mm 以上の余裕があり、XY 移動による引っ張りを吸収します。
- 送り出し量はスプールの残量（巻き径）で変わりますが、この余裕で吸収されます。

### 4. 駆動系の幾何

| 項目 | 値 | 根拠 |
|:--|:--|:--|
| プーリーのピッチ径 | 12.73 mm | 20歯 × 2mm ÷ π |
| ローラー上のベルト心線の径 | 約 32.0 mm | φ30 ＋ 2 ×（歯高 0.75 ＋ ピッチライン 0.254） |
| 角速度の減速比 | 約 2.51 | 32.0 ÷ 12.73（歯がウレタンに食い込む程度により 2.40〜2.51） |
| モーター1回転あたりのローラー表面移動 | 約 37.5 mm | 40 mm × 15 ÷ 16.0 |
| ローラー表面 1 mm あたりのステップ数 | 約 85.4 | 3200 ステップ/回転（1/16）÷ 37.5 mm |

スプールの縁の径は一定なので、スプールの回転速度は一定です。フィラメントの送り速度は「ローラー表面速度 ×（巻き径 ÷ 縁の径）」で、スプールの残量によって変わります（例：縁 φ180、芯 φ80 のスプールでは 0.44〜1.0 倍）。

### 5. 送り制御の値

2023年版ファームの値をそのまま使っています。旧ファームは `move()` を毎ループ呼ぶため、実際の動きは設定値と異なっていました。新ファームは、その実際の動きを明示的な値で再現し、スイッチが切れた瞬間の速度の跳ね上がりだけを取り除いています（AVR シミュレーションで確認。[tools/sim](../tools/sim/)）。

| 項目 | 旧ファームの設定値 | 旧ファームの実際の動き | 新ファーム |
|:--|:--|:--|:--|
| 最高速度（スイッチ ON 中） | 4000 ステップ/s | 約 2,390 ステップ/s | 2400 ステップ/s（実測 約 2,360） |
| 立ち上がりの加速度 | 2000 ステップ/s² | 約 3,800 ステップ/s² | 4000 ステップ/s² |
| スイッチ OFF 直後 | ― | 約 3,900 ステップ/s に跳ね上がり | 跳ね上がりなし |
| OFF 後の減速度 | 2000 ステップ/s² | 約 1,960 ステップ/s² | 2000 ステップ/s² |
| OFF 後の送り | 4000 ステップ | 4000 ステップ | 4000 ステップ |
| マイクロステップ、電流 | 1/16、600 mA RMS、stealthChop | 同左 | 同左（TMC2130 のレジスタ値も同一） |

2400 ステップ/s はローラー表面で約 28 mm/s、4000 ステップはローラー表面で約 47 mm です。

### 6. 保護機能

**ジャム監視（ATtiny85、メインの Pro Mini から独立）**

- 送り要求（スイッチ ON）が 48 秒連続したら、リレーでモーター電源（VM）を切り、停止を保持します。48 秒は 2023 年版の実効値に合わせています。
- 20 ms 未満の OFF（接点のばたつき）では計時をリセットしません。
- ATtiny85 自身が停止した場合は、ウォッチドッグでリセットしたうえで停止を保持します。
- 停止中は出力をハイインピーダンスにし、プルダウン抵抗でリレーを OFF にします。
- リセットスイッチ（X3）は Pro Mini と ATtiny85 の両方をリセットし、停止の保持を解除します。
- 改版基板（rev.B）では外部入力（X5）を ATtiny85 に接続します。外部センサーが異常（LOW または断線）を示したときも、同じようにモーター電源を切ります。

**ドライバ監視（Pro Mini）**

- TMC2130 の設定を書き込んだ後に読み戻して確認し、確認できてからドライバを有効にします。
- 500 ms ごとに TMC2130 の状態を確認します。モーター電源の瞬断などでドライバがリセットされた場合は、自動的に設定し直します。
- 過熱・短絡を検出したら、ドライバを止めて保持します。
- 送り要求が 53 秒続いた場合も、ソフトウェア側でモーターを止めて保持します（ATtiny85 の予備）。

---

## English

### 1. Problem

On a direct-drive printer the extruder pulls the filament off the spool, so the spool inertia and friction load the extruder directly.

The largest load comes from long XY travel moves of the nozzle. The filament is pulled by the travel distance, regardless of how much is being extruded.

### 2. Construction

```
Stepper motor 17HS08-1004S (1.8 deg)
  └ GT2 20T pulley ─ GT2 closed-loop belt 188 mm ─ urethane roller dia. 30 mm (no pulley; the belt runs directly on the roller)
       └ the spool flange (outer rim) rests on the roller and is driven by friction
            └ filament ─ lever with elastic cord (switch) ─ printer
```

### 3. Feed control

- When the lever held by the elastic cord is pulled beyond a threshold, the switch closes and the motor pays out filament.
- After the switch opens, the motor runs 4000 more steps and stops.
- More filament is paid out than consumed. The elastic cord and the slack give more than 200 mm of buffer, which absorbs the pull of XY travel moves.
- The amount paid out changes with the remaining filament (winding diameter); the buffer absorbs this.

### 4. Drive geometry

| Item | Value | Basis |
|:--|:--|:--|
| Pulley pitch diameter | 12.73 mm | 20 teeth × 2 mm ÷ π |
| Belt pitch-line diameter on the roller | about 32.0 mm | 30 + 2 × (tooth height 0.75 + pitch line 0.254) |
| Angular reduction ratio | about 2.51 | 32.0 ÷ 12.73 (2.40 to 2.51 depending on how far the teeth sink into the urethane) |
| Roller surface travel per motor revolution | about 37.5 mm | 40 mm × 15 ÷ 16.0 |
| Steps per mm of roller surface | about 85.4 | 3200 steps/rev (1/16) ÷ 37.5 mm |

The spool flange diameter is constant, so the spool turns at a constant speed. The filament feed speed is "roller surface speed × (winding diameter ÷ flange diameter)" and changes with the remaining filament (e.g. 0.44 to 1.0 for a spool with a 180 mm flange and an 80 mm core).

### 5. Feed control values

The values of the 2023 firmware are kept. Because the old firmware called `move()` on every loop pass, its real motion differed from the settings. The new firmware reproduces that real motion with explicit values and only removes the speed jump at the moment the switch opens (verified in AVR simulation, see [tools/sim](../tools/sim/)).

| Item | Old setting | Old real motion | New firmware |
|:--|:--|:--|:--|
| Top speed (switch on) | 4000 steps/s | about 2,390 steps/s | 2400 steps/s (measured about 2,360) |
| Ramp-up acceleration | 2000 steps/s² | about 3,800 steps/s² | 4000 steps/s² |
| Right after the switch opens | – | jumps to about 3,900 steps/s | no jump |
| Deceleration after the switch opens | 2000 steps/s² | about 1,960 steps/s² | 2000 steps/s² |
| Feed after the switch opens | 4000 steps | 4000 steps | 4000 steps |
| Microstepping, current | 1/16, 600 mA RMS, stealthChop | same | same (identical TMC2130 register values) |

2400 steps/s is about 28 mm/s at the roller surface; 4000 steps is about 47 mm at the roller surface.

### 6. Protection

**Jam monitor (ATtiny85, independent of the main Pro Mini)**

- If the feed request (switch on) lasts 48 s without a break, a relay cuts the motor supply (VM) and the stop is latched. 48 s matches the effective value of the 2023 firmware.
- Off periods shorter than 20 ms (contact bounce) do not reset the timer.
- If the ATtiny85 itself hangs, the watchdog resets it and the stop is latched.
- While stopped, the output is high impedance and a pull-down resistor keeps the relay off.
- The reset switch (X3) resets both the Pro Mini and the ATtiny85 and clears the latched stop.
- On board rev.B, the external input (X5) is connected to the ATtiny85. When an external sensor reports a fault (LOW or broken wire), the motor supply is cut in the same way.

**Driver monitor (Pro Mini)**

- The TMC2130 configuration is read back and verified before the driver is enabled.
- The TMC2130 is checked every 500 ms. If the driver was reset (e.g. by a short interruption of the motor supply), it is reconfigured automatically.
- On overtemperature or short circuit, the driver is stopped and the stop is latched.
- If the feed request lasts 53 s, the software also stops the motor and latches (backup of the ATtiny85).
