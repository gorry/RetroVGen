◇ RetroVGen : Retro RGB Video Signal Generator
  Hiroaki GOTO as GORRY / http://GORRY.hauN.org/
  2022/01/05 Version 20220105a

========================================================================

1. 概要

RetroVGenは、Arduino Unoを使用してレトロPCなどによるRGB映像出力を模倣した
信号を生成します。
水平同期周波数15/24/31kHz、最大16種類の出力をサポートしています。

========================================================================

2. 使用方法

パッケージ内のHEXファイルをArduino Unoに書き込み、電源をONにすることで、
画面モードSWで設定した番号の映像信号を出力します。

HEXファイルを書き込むときは、動作周波数が16MHzになるようにFuseビットを
設定してください。8MHzでは正しく動作しません。

========================================================================

3. 画面モード

画面モードSWを変更することで、画面モードを切り替えることができます。
SW1-4で合計16種類の画面モードを持つことができます。

  +-------+-----------------------------------------------+
  | SW1-3 | SW4                                           |
  |       +-----------------------+-----------------------+
  |       | 0                     | 1                     |
  +-------+-----------------------+-----------------------+
  | 0     | 0:VGA 31kHz           | 8:VGA 31kHz           |
  |       | H=31.50kHz/V=60.00Hz  | H=31.50kHz/V=60.00Hz  |
  +-------+-----------------------+-----------------------+
  | 1     | 1:NTSC 15kHz          | 9:PAL 15kHz           |
  |       | H=15.70kHz/V=59.94Hz  | H=15.63kHz/V=50.08Hz  |
  +-------+-----------------------+-----------------------+
  | 2     | 2:SHARP X1 15kHz      | A:SHARP X1 15kHz      |
  |       | H=15.98kHz/V=61.94Hz  | H=15.98kHz/V=61.94Hz  |
  +-------+-----------------------+-----------------------+
  | 3     | 3:SHARP X68000 15kHz  | B:SHARP X68000 15kHz  |
  |       | H=15.98kHz/V=61.46Hz  | H=15.98kHz/V=61.46Hz  |
  +-------+-----------------------+-----------------------+
  | 4     | 4:NEC PC-9801 15kHz   | C:NEC PC-9801 15kHz   |
  |       | H=15.98kHz/V=61.23Hz  | H=15.98kHz/V=61.23Hz  |
  +-------+-----------------------+-----------------------+
  | 5     | 5:SHARP X1turbo 24kHz | D:SHARP X1turbo 24kHz |
  |       | H=24.86kHz/V=55.49Hz  | H=24.86kHz/V=55.49Hz  |
  +-------+-----------------------+-----------------------+
  | 6     | 6:NEC PC-9801 24kHz   | E:NEC PC-9801 24kHz   |
  |       | H=24.83kHz/V=56.42Hz  | H=24.83kHz/V=56.42Hz  |
  +-------+-----------------------+-----------------------+
  | 7     | 7:SHARP X68000 31kHz  | F:SHARP X68000 31kHz  |
  |       | H=31.50kHz/V=55.46Hz  | H=31.50kHz/V=55.46Hz  |
  +-------+-----------------------+-----------------------+

現在8/A-Fは空きとなっており、0/2-7のコピーとなっています。
Amigaなどの機種の映像信号パラメータを入れることがあるかもしれません。

========================================================================

4. ハードウェア

本プログラムは、Arduinoシステムに専用のシールドを接続して使用する想定と
なっています。回路図はBSch3Vで書かれており、本プロダクト配布ページに
設置されています。BSch3Vについては、以下を参照してください。
https://www.suigyodo.com/online/schsoft.htm

概略としては以下のようになります。

・D1(TX)
  シリアル出力を利用して、R/G/B映像を模倣します。
  150Ω抵抗を経由して、VGAコネクタ1-3pへ接続します。

・D3
  VSYNC信号となります。
  68Ω抵抗を経由して、VGAコネクタ14pへ接続します。

・D8
  VSYNCカウンタ0～3の間、LEDが点灯します。
  画面表示と比べることで、表示遅延を確認することができます。

・D10
  OC1によるFast PWM出力を利用して、HSYNC信号を模倣します。
  68Ω抵抗を経由して、VGAコネクタ13pへ接続します。

・D11-13, D9
  画面モードSWとなります。

Arduino Unoを使用せず、ATMega328Pを直接利用しての動作も確認しています。

========================================================================

5. ソフトウェア

ソースコードは、Arduino IDE 2.0.3でのビルドを確認しています。
HEXファイルでなくソースコードからビルドして書き込む場合は、以下の点に
注意してください。

・コード内の定数は、ATMEGA 328pを16MHzで動かすことを想定した値になって
  います。16MHzで動作するよう、Fuseビットを設定してください。

・IDEやライブラリのバージョンが変更された場合、水平同期期間で動かせる
  ユーザーコードの実行期間が少なくなり、現在の定数では正しく動作しなく
  なる可能性があります。
  D8に繋がっているVSYNC LEDの周期を確認てください。正常ならば1Hz程度の
  周期で点滅していますが、0.5Hzの遅さになっている場合は、この問題が発生
  しています。
  videoparam.h の「Horizontal Chars」を減らすことで、この問題を回避する
  ことができます。

========================================================================

6. 制限事項

・HSYNCのタイミングは、クロック分解能の制限により、実機と比較して最大
  0.1%程度の誤差があります。また、VSYNCはHSYNCの整数倍のタイミングとな
  るため、同様に最大0.1%程度の誤差となります。

・水平表示期間（HDISP）内でのコンテンツ（文字）の表示タイミングは、実機
  と同一ではありません。このため、「RetroVGenで画面左右が切れていなくて
  も、実機では切れる」「表示部の縦横比が異なる」などの可能性があります。
  表示が乱れていないことを確認するための参考程度とお考えください。
  （多くのモニタで「横方向の表示位置や表示幅は調整可能」であるため、表
  示の可否には大きく影響しない可能性が高いです）

========================================================================

7. 著作権表記

本プロダクトは、Nick Gammon氏による「VGA video generation」からの派生で
制作されています。当方は本プロダクトの著作権を保有しますが、Nick Gammon
氏の著作権表記を引き継ぎます。詳細はoriginal.hをご覧ください。

本プロダクトは、自由かつ無償で使用・コピー・配布・変更・流用を行うことが
できます。また許可なく再配布・出版・販売を行うことができます。

本プロダクトは、無保証です。使用した、あるいはしなかったことによる一切の
責任は使用者にあるものとします。

本プロダクトは、以下URLを配布先とします。利便性などのためにこれ以外の
URLで配布することがありますが、以下が最も正式なものであり、完全な最新の
パッケージを得ることができます。

http://GORRY.hauN.org/pw/?RetroVGen

========================================================================

8. 連絡先

後藤 浩昭 / GORRY
http://GORRY.hauN.org/
twitter/gorry5

========================================================================
[EOF]
