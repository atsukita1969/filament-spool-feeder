# Direct type 3D printer filament feeder
ダイレクト式3Dプリンタの問題点：  
*スプールが重いと、フィラメントの引き込みに抵抗がかかり品質に影響する  
特に重いスプール（1ｋｇ）  
*早いスピードでプリントをすると、引き込みに抵抗がかかり吐出量が減る  
*急に遠い方向に位置が変わるとフィラメントが戻る。  
  
広い部屋でプリントできる場合は、スプールと3Dプリンタの距離を遠くする(遊びのフィラメントを長く取る)ことでこの品質低下を防ぐことが出来る。  
しかしホビー用途ではそんな広い場所は無理。
（Prusaの動画で、このことに気付いた）


ボーデン式とダイレクト式を合わせたシステムが作れないか調査し、このシステムを製作した。  
  
  

# Machine
機械の基本仕様:  
*スプールをウレタンローラーに乗せて、ステッピングモーターで回す  
*供給(モーター回転)のタイミングは、フィラメントのテンションが強くなったとき（遊びが少なくなった時）  
*フィラメントのテンションは、ゴム紐とリミットスイッチで検出  
*連続して一定時間以上モーターが回転する場合は、システムダウン（ジャム防止。3Dプリントは、フィラメントが切れてダメになる）  
(↑3Dプリンタと連動させたい。プリンタ側に一時停止のI/Oがあれば可能）  
![](https://user-images.githubusercontent.com/44044800/230771700-191599d2-2b6f-4a53-9985-95916052d0fc.jpg)

# Electric board  
ベースはArduino Pro-mini  
ステッピングモーターのドライバは、TMC2130を使用する。（たまたま手持ちがあったため）  
ジャム防止のシステムダウンには、ATTINY85をArduino化してモニタリングさせる。  

# Software

ArduinoIDE


