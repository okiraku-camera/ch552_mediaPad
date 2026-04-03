# mediaPad firmware

English version is here: [README_en.md](README_en.md)

このディレクトリには、CH552G 向けの mediaPad ファームウェア（Arduino スケッチ）が入っています。

## 概要

mediaPad は USB 接続で動作するマクロパッドです。  
ファームウェアは、キースキャン・ロータリーエンコーダ入力・USB HID 送信・設定保存（EEPROM）を担当します。

## 主な動作

- 2x4 の 8 キー入力をスキャンして、割り当て済みのキーコードを送信
- ロータリーエンコーダの回転方向（CW / CCW）に応じてキーコードを送信
- 2 レイヤーのキーマップを EEPROM から読み出して使用
- 初回起動時やリセット時にデフォルトキーマップを EEPROM へ書き込み
- マクロデータを EEPROM に保存し、対応キーで順次送信
- WebHID 経由の設定コマンドを受け取り、キーマップやマクロを更新

## 主要ファイル

- `mediaPad.ino`: メイン処理（入力スキャン、キー送信、レイヤー/マクロ制御）
- `USB_hid_composite.c/.h`: USB HID の送受信処理
- `hid_raw_request.c`: WebHID からの設定要求（読み書きコマンド）処理
- `nvm_keymap.c/.h`: EEPROM へのキーマップ/マクロ保存処理
- `r_encoder.c/.h`: ロータリーエンコーダ入力処理
- `serial_led.c/.h`: LED 表示制御
- `hardware_config.h`: キー数や EEPROM マップなどの定数定義

## ビルドと書き込み

1. Arduino IDE を用意し、CH552G 用ボード定義を導入します。
2. この `mediaPad.ino` を開き、ボードを CH552G に設定します。
3. ビルドオプションの USB Settings は **USER CODE w/ 148B USB ram** を選択します（必須）。
4. 通常の Arduino と同様にビルドして書き込みます。

### USB Settings オプションの意味

- **USER CODE**: USB の制御処理を既定クラスではなくユーザー実装コードで扱う設定です。本プロジェクトの独自 HID 処理（複合 HID と WebHID 通信）を有効にするために必要です。
- **148B USB ram**: CH552 の USB 専用 RAM 領域として 148 バイトを割り当てる設定です。USB エンドポイントのバッファ確保に使われ、通信を安定して動かすために必要です。
## 補足

Web 設定ツールは上位ディレクトリの [html/ch552_mediaPad.html](../html/ch552_mediaPad.html) を使用します。  
ここからキーマップ・マクロの読み書きやリセットが可能です。
