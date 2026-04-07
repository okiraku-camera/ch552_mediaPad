# CH552 MediaPad

English version: [README_en.md](README_en.md)

CH552G マイクロコントローラを使ったUSB HID マクロパッド（2x4キー + エンコーダ搭載）です。WebHID を使ってキーマップをリアルタイムで設定・変更できます。

## フォルダ構成

- **`pcb/`**: KiCAD 8 による基板設計（回路図、PCB）
- **`plates/`**: レーザーカット用アクリルプレート設計
- **`mediaPad/`**: Arduino ファームウェア（CH552G 向け）
- **`html/`**: WebHID 設定用ブラウザアプリ（HTML/CSS/JavaScript）
- **`LICENSE`**: プロジェクトライセンス

## ハードウェア

- **マイコン**: CH552G
- **キー**: 8キー（2x4 レイアウト）
- **エンコーダ**: 1個
- **トッププレート**: 5mm アクリル（透明）
- **ミドルプレート**: 5mm アクリル（マット透明）
- **ボトムプレート**: 2mm アクリル（透明）
- **USB**: USB Type-C コネクタ

詳細は [pcb/README.md](pcb/README.md)・[plates/README.md](plates/README.md) をご覧ください。

## ソフトウェア

### ファームウェア (mediaPad/)
CH552G 向けの Arduino スケッチ。USB HID キーボード・メディア操作・エンコーダ入力に対応しています。

### Web アプリ (html/)
ブラウザから **WebHID** 経由でデバイスと通信し、キーマップの読み書きや設定変更ができるアプリケーションです。(ch552_mediaPad.html)

## ライセンス

このプロジェクトはハードウェアとソフトウェアから構成されています。

Copyright (c) Takeshi Higasa, okiraku-camera.tokyo

詳細は [LICENSE](./LICENSE) をご覧ください。


