# Mechanical parts / 機構部品

[日本語](#日本語) | [English](#english)

---

## 日本語

| ファイル | 内容 |
|:--|:--|
| [step/assy_top_dual.stp](step/assy_top_dual.stp) | 2スプール版の全体組立（Autodesk Inventor から出力した STEP） |
| [step/parts/](step/parts/) | 自作部品の STEP（Base_lever_L/R、lever_L/R、MotorMount、tri_Bracket） |
| [bom.csv](bom.csv) | 部品表 |

自作部品は精度を必要としないので、3D プリンタでの製作を想定しています。

駆動系: モーター（17HS08-1004S）の GT2 20歯プーリーから、GT2 無端ベルト 188mm をウレタンローラー（外径30mm）に直接巻き掛けています。ローラー側にプーリーはありません。詳しくは [docs/principle.md](../../docs/principle.md) を参照してください。

---

## English

| File | Contents |
|:--|:--|
| [step/assy_top_dual.stp](step/assy_top_dual.stp) | Complete assembly of the two-spool version (STEP exported from Autodesk Inventor) |
| [step/parts/](step/parts/) | STEP files of the self-made parts (Base_lever_L/R, lever_L/R, MotorMount, tri_Bracket) |
| [bom.csv](bom.csv) | Bill of materials |

The self-made parts need no high precision and are meant to be 3D printed.

Drive: a GT2 20T pulley on the motor (17HS08-1004S) drives a GT2 closed-loop belt of 188 mm that runs directly on the urethane roller (outer diameter 30 mm). There is no pulley on the roller. See [docs/principle.md](../../docs/principle.md).
