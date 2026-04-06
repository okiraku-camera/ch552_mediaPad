# mediaPad firmware

English version is here: [README_en.md](README_en.md)

このディレクトリには、mediaPad ファームウェア（Arduino スケッチ）が入っています。

## 概要

mediaPad は、CH552G を使った **プログラマブルなメディアパッド/マクロパッド** です。  
キースキャン、ロータリーエンコーダ入力、USB HID 送信、キーマップ/マクロの永続保存を行い、WebHID ベースの WebUI から設定を変更できます。

このファームウェアの対象ハードウェアは **ch552_mediaPad** です。


## 主な機能

- **8 キー + ロータリーエンコーダ入力**: 2x4 のキー入力とエンコーダ回転を処理
- **2 レイヤーキーマップ**: EEPROM 上の 2 層キーマップを切り替えて利用
- **エンコーダ割り当て**: レイヤーごとに CW / CCW のキーコードを設定可能
- **マクロ機能**: 最大 6 個のマクロを保存し、割り当てキーから順次送信
- **WebUI 設定**: WebHID 経由でキーマップ、エンコーダ、マクロをブラウザから編集可能
- **永続ストレージ**: 設定は NVM（内蔵 EEPROM）に保存

## ソフトウェア構成

### ハードウェア層
| ファイル | 役割 |
|---------|------|
| `mediaPad.ino` | メインスケッチ。入力スキャン、キー送信、レイヤー/マクロ制御 |
| `r_encoder.c/.h` | ロータリーエンコーダ入力処理 |
| `serial_led.c/.h` | LED 表示制御 |

### レイヤー・キーマップ管理
| ファイル | 役割 |
|---------|------|
| `nvm_keymap.c/.h` | EEPROM へのキーマップ/マクロ保存処理 |
| `hardware_config.h` | キー数、レイヤー数、EEPROM マップなどの定数定義 |

### USB HID 通信
| ファイル | 役割 |
|---------|------|
| `USB_hid_composite.c/.h` | USB HID 複合デバイス実装 |
| `hid_raw_request.c` | WebHID 設定要求（読み書きコマンド）処理 |

### PC側 Web アプリケーション
| ファイル | 役割 |
|---------|------|
| `html/ch552_mediaPad.html` | Web アプリケーション画面 |
| `html/ch552_mediaPad.js` | WebHID 通信と UI ロジック |
| `html/style.css` | スタイル定義 |

## 動作モード

### レイヤー切り替え
- 2 レイヤーのキーマップを持ちます
- SW7 は固定 Fn キーとして扱われ、通常のキー割り当て変更対象ではありません
- Fn 操作により現在レイヤーを切り替えます

### エンコーダ動作
- ロータリーエンコーダ 1 個を搭載した構成です
- レイヤーごとに CW / CCW の 2 方向へ別々のキーコードを割り当てられます
- 既定ではメディア操作やスクロール系コードを割り当てる運用を想定しています

### マクロ動作
- マクロキー押下で登録済みマクロを順次送信します
- 実行中に別のキー入力があると、送信中マクロは停止します
- 遅延コードと繰返し制御コードを含むマクロを実行できます

## 使い方

### クイックスタート
1. ブラウザで `html/ch552_mediaPad.html` を開きます。
2. 「キーボードに接続」を押してデバイスを選択します。
3. 必要に応じてキーマップ、エンコーダ割り当て、マクロを編集します。
4. 「キーマップを書き込む」「マクロを書き込む」でデバイスへ保存します。

### WebUI の詳細手順

WebUI の操作手順は、以下の分離ドキュメントにまとめています。

- 日本語: `html/README.md`
- English: `html/README_en.md`

## ビルド方法

このファームウェアのビルドには、以下のソフトウェアが必要です。

- Visual Studio Code + vscode-arduino 拡張
- arduino-cli
- CH55xDuino

### arduino.json によるデバイス切り分け

本プロジェクトでは、デバイスごとのビルド依存設定を各スケッチディレクトリの `.vscode/arduino.json` で分離しています。

- `macro_numpad/.vscode/arduino.json`:
	- `sketch`: `macro_numpad.ino`
	- `buildPreferences`: `-DBOARD_NAME=CH552_MACRO_NUMPAD -I../common`
- `mediaPad/.vscode/arduino.json`:
	- `sketch`: `mediaPad.ino`
	- `buildPreferences`: `-DBOARD_NAME=CH552_MEDIAPAD -I../common`

この構成により、`software/common` の共通モジュールを再利用しながら、ターゲットごとのハードウェア依存データをボード名を使ったマクロで切り分けています。`.vscode/arduino.json` は以下のようになっています。

```json
{
    "configuration": "clock=16internal,usb_settings=user148,upload_method=usb,bootloader_pin=p36",
    "board": "CH55xDuino:mcs51:ch552",
    "sketch": "mediaPad\\mediaPad.ino",
    "output": "./built",
    "port": "COM1",
    "buildPreferences": [
        ["compiler.cpp.extra_flags","-DCH552_MEDIAPAD"],
        ["compiler.c.extra_flags","-DCH552_MEDIAPAD"]
    ]
}
```

VS Code の vscode-arduino 拡張は内部で arduino-cli を利用し、これらの設定を使ってビルドします。


### ボード設定

ボード設定メニューで以下を選択してください。

- USB Settings: `USER CODE w/ 148B USB ram`

この設定は HID エンドポイントに必要な USB RAM を確保するために必要です。

## 技術詳細

### NVM（128バイト）の使用方法
- **0～1バイト**: キーマップ有効フラグ（`0xAA55`）
- **2～33バイト**: 2 レイヤー分のキーキーマップ（8 キー x 2 バイト x 2 レイヤー）
- **34～41バイト**: ロータリーエンコーダ割り当て（1 個 x CW/CCW x 2 レイヤー x 2 バイト）
- **42～126バイト**: マクロ文字列データ
- **127バイト**: マクロ領域終端（予約）

### マクロ機能
- **最大 6 マクロ** を登録可能
- **マクロ領域は 85 バイト** です
- 1 マクロストリングの UI 上限は **50 バイト** です
- 各マクロはゼロ終端の連続列として管理されます

### マクロ制御コード
- **遅延コード**:
	- `0xDC` = 100ms
	- `0xDD` = 500ms
	- `0xDE` = 1sec
- **繰返しコード**:
	- `0xDA` = SEGSTART
	- `0xDB` = GOTO_SEG

### 繰返し動作の仕様
- `GOTO_SEG` は、そのマクロ内で最初に見つかった `SEGSTART` 位置へ戻ります
- `SEGSTART` が存在しない場合は、マクロ先頭へ戻ります

## トラブルシューティング

### デバイスが接続できない
- ブラウザが WebHID をサポートしているか確認してください（Chrome、Edge 推奨）
- デバイスが WebHID 通信可能な状態で接続されているか確認してください
- ブラウザの USB アクセス権限を確認してください

### キーマップやエンコーダ設定が反映されない
- 「キーマップを書き込む」で変更を保存しているか確認してください
- Pending 状態の変更は、読み出しだけでは反映されません
- 一度「キーマップを読み出す」を実行してから再編集すると切り分けしやすくなります

### マクロが期待どおり動作しない
- マクロスロットと割り当て先キーが正しいか確認してください
- マクロ容量上限を超えていないか確認してください
- 遅延コードや繰返しコードを意図どおりに配置しているか確認してください

## 補足

- Web 設定ツールは [html/ch552_mediaPad.html](html/ch552_mediaPad.html) を使用します
- 接続時にデバイスのバージョン番号を確認しています（想定値: `0x1234`）
- ブートローダー開始後はデバイスが一時的に切断されることがあります
