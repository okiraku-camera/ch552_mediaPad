# PCB (ch552_mediaPad)

このディレクトリには、CH552 MediaPad の KiCad 設計データ（回路図・基板レイアウト）が含まれています。

## 使用ツール

- KiCad 8.0

## 含まれる主なファイル

- `ch552_mediaPad.kicad_sch` : 回路図
- `ch552_mediaPad.kicad_pcb` : PCB レイアウト
- `ch552_mediaPad.kicad_pro` : プロジェクト設定
- `local/` : この PCB のためのフットプリントライブラリ

### local/ ディレクトリの内容

- `Hole_M3_SmallPad.kicad_mod` : M3 取り付け穴
- `MX_PCB_1.25u.kicad_mod` : 1.25u キースイッチ用フットプリント
- `MX_Plate_hole.kicad_mod` : MX スイッチ用フットプリントと置き換えて、スイッチプレートを設計するためのフットプリント
- `SW_Cherry_MX_PCB_RE_1.25u.kicad_mod` : MX 1.25U とロータリーエンコーダー EC-12 を組み合わせたフットプリント
- `USB_C_Receptacle_304J-BCP16.kicad_mod` : USB Type-C コネクタ

## メモ

- 共同作業時は、編集対象（回路図/PCB）と合わせて関連設定ファイルの差分も確認してください。
