# Firmware simulation / ファームのシミュレーション検証

[日本語](#日本語) | [English](#english)

---

## 日本語

コンパイル済みのファーム（.elf）を [simavr](https://github.com/buserror/simavr) で命令単位に実行し、旧版（2023年）と新版を比較するための環境です。

| ファイル | 内容 |
|:--|:--|
| `sim328.c` | Pro Mini（ATmega328P 16MHz）と TMC2130 の SPI 動作モデル。STEP パルス、EN、LED、TMC2130 レジスタの書き込みを記録 |
| `simtiny.c` | ATtiny85（8MHz）。PB1（送り要求）と PB4（外部入力）を操作し、PB3（リレー駆動）を監視 |
| `analyze.py` | 速度プロファイルの解析（立ち上がり加速度、最高速度、解除後の送り） |
| `ledsum.py` | LED の点灯・点滅パターンの要約 |
| `scenarios/` | 試験シナリオ（1行 = `時刻ms イベント 値`、最後の行は `時刻ms END 0`） |

TMC2130 モデルは、VM（モーター電源）が切れている間は SPI に応答せず、VM が入ると全レジスタが 0 になって GSTAT.reset が立ちます（モジュールのロジック部が VM から給電される、最も厳しい場合を想定）。simavr 1.6 は SPI の1バイトを 100µs として扱うため、SPI 通信中の時間は実機より長く出ます。

### 使い方（Ubuntu）

```
sudo apt install simavr libsimavr-dev libelf-dev
gcc -O2 -o sim328 sim328.c -I/usr/include/simavr -lsimavr -lelf -lm
gcc -O2 -o simtiny simtiny.c -I/usr/include/simavr -lsimavr -lelf -lm
arduino-cli compile -b arduino:avr:pro:cpu=16MHzatmega328 --output-dir out ../../firmware/feeder_main
./sim328 out/feeder_main.ino.elf scenarios/scA.txt log.txt
python3 analyze.py out/feeder_main.ino.elf scenarios/scA.txt 1.0 4.0
```

イベント: `sim328` は `D4`（送り要求）、`VM`（モーター電源）、`ERR`（TMC2130 の過熱・短絡）。`simtiny` は `REQ`（送り要求）、`EXT`（外部入力）。

### 結果（2026年9月）

| シナリオ | 内容 | 旧版 | 新版 |
|:--|:--|:--|:--|
| scA | 1〜4秒 送り要求 | ON中 2,386 ステップ/s、解除直後 3,900 へ跳ね上がり、解除後 4,000 ステップ | ON中 約2,356、跳ね上がりなし、解除後 約4,000 ステップ。TMC2130 レジスタ値は同一 |
| scShort | 0.3秒 送り要求 | 解除後 2,900 ステップ/s まで加速、合計 4,200 ステップ | 2,400 で頭打ち、合計 4,209 ステップ |
| scRepress | 解除 0.6秒後に再要求 | 3,900 へ跳ね上がった後 2,340 へ急減速 | 一定速度 |
| scLateVM | VM を 0.5秒遅れて投入 | 回らない（設定が消える） | 0.51秒で設定完了 |
| scGlitch | 送り中に VM を 50ms 遮断 | 以後回らない | 2.51秒に再設定して復帰 |
| scVMcut | 送り中に VM を遮断したまま | ― | LED 遅い点滅 |
| scErr | 送り中に TMC2130 異常 | ― | 2.00秒に停止保持、LED 速い点滅 |
| scJam60 | 送り要求を 59秒保持 | 回り続ける | 要求から 53秒で停止保持 |
| t1 | ATtiny85: 連続要求 | 47.9秒でリレー OFF | 48.0秒でリレー OFF |
| t2 | ATtiny85: 短い要求 200回 | 31.5% の解除でカウンタが戻らない（停止はせず） | 誤停止なし |
| t3 | ATtiny85: 25秒の要求 12回 | 2回目で誤停止 | 誤停止なし |
| t4 | ATtiny85: 1秒ごとに 5ms のばたつき | ― | 48秒で停止 |
| t5 | ATtiny85: 30ms の OFF の後も要求継続 | ― | OFF でリセットされ、その 48秒後に停止 |
| e1〜e4, e2b | ATtiny85 rev.B: 外部入力 | ― | 30ms の LOW では止まらず 80ms で停止、起動時に正常にならなければリレーを入れずに保持、ジャム判定は同じ |

ウォッチドッグは、`loop()` を意図的に止めた試験用ビルドで、約1秒後にリセットされ停止を保持することを確認しました。

---

## English

An environment that runs the compiled firmware (.elf) instruction by instruction in [simavr](https://github.com/buserror/simavr) to compare the old (2023) and new versions.

| File | Contents |
|:--|:--|
| `sim328.c` | Pro Mini (ATmega328P 16 MHz) with an SPI model of the TMC2130. Records STEP pulses, EN, LED and TMC2130 register writes |
| `simtiny.c` | ATtiny85 (8 MHz). Drives PB1 (feed request) and PB4 (external input), watches PB3 (relay drive) |
| `analyze.py` | Speed profile analysis (ramp-up acceleration, top speed, feed after release) |
| `ledsum.py` | Summary of the LED on/blink pattern |
| `scenarios/` | Test scenarios (one line = `time_ms EVENT value`, last line `time_ms END 0`) |

The TMC2130 model does not answer on SPI while VM (motor supply) is off; when VM comes on, all registers are 0 and GSTAT.reset is set (worst case: the module logic is supplied from VM). simavr 1.6 treats one SPI byte as 100 µs, so time spent in SPI transfers is longer than on real hardware.

### Usage (Ubuntu)

```
sudo apt install simavr libsimavr-dev libelf-dev
gcc -O2 -o sim328 sim328.c -I/usr/include/simavr -lsimavr -lelf -lm
gcc -O2 -o simtiny simtiny.c -I/usr/include/simavr -lsimavr -lelf -lm
arduino-cli compile -b arduino:avr:pro:cpu=16MHzatmega328 --output-dir out ../../firmware/feeder_main
./sim328 out/feeder_main.ino.elf scenarios/scA.txt log.txt
python3 analyze.py out/feeder_main.ino.elf scenarios/scA.txt 1.0 4.0
```

Events: `sim328` uses `D4` (feed request), `VM` (motor supply), `ERR` (TMC2130 overtemperature/short). `simtiny` uses `REQ` (feed request) and `EXT` (external input).

### Results (September 2026)

| Scenario | Description | Old | New |
|:--|:--|:--|:--|
| scA | Feed request 1–4 s | 2,386 steps/s while requested, jumps to 3,900 at release, 4,000 steps after release | about 2,356, no jump, about 4,000 steps after release. Identical TMC2130 register values |
| scShort | Feed request 0.3 s | Accelerates to 2,900 steps/s after release, 4,200 steps in total | Capped at 2,400, 4,209 steps in total |
| scRepress | New request 0.6 s after release | Jumps to 3,900, then drops abruptly to 2,340 | Constant speed |
| scLateVM | VM switched on 0.5 s late | Does not move (configuration lost) | Configured at 0.51 s |
| scGlitch | VM interrupted for 50 ms while feeding | Never moves again | Reconfigured and recovered at 2.51 s |
| scVMcut | VM cut while feeding and left off | – | LED slow blink |
| scErr | TMC2130 fault while feeding | – | Stop latched at 2.00 s, LED fast blink |
| scJam60 | Feed request held for 59 s | Keeps running | Stop latched 53 s after the request |
| t1 | ATtiny85: continuous request | Relay off at 47.9 s | Relay off at 48.0 s |
| t2 | ATtiny85: 200 short requests | Counter not reset on 31.5% of releases (no stop) | No false stop |
| t3 | ATtiny85: 12 requests of 25 s | False stop on the second request | No false stop |
| t4 | ATtiny85: 5 ms bounce every second | – | Stops at 48 s |
| t5 | ATtiny85: 30 ms off, request continues | – | Reset by the off period, stops 48 s later |
| e1–e4, e2b | ATtiny85 rev.B: external input | – | No stop on 30 ms LOW, stop on 80 ms; relay stays off and latched if the input is not OK at start-up; jam detection unchanged |

The watchdog was checked with a test build that deliberately stops `loop()`: the MCU is reset after about 1 s and the stop is latched.
