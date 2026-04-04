# MediaPad Web Config

This directory contains the Web configuration app for CH552 MediaPad.  
Using WebHID in the browser, you can read and write the device keymap, encoder assignments, and macros.

## Files

- `ch552_mediaPad.html`: Main UI page
- `ch552_mediaPad.js`: WebHID communication and UI logic
- `style.css`: Stylesheet

## Features

- Device connection via WebHID
- Keymap read/write (2 layers)
- Rotary encoder assignment read/write (CW / CCW)
- Macro read/write
- Macro editor dialog (add, delete, clear, storage usage display)
- Reset keymap to defaults
- Start bootloader
- Read NVM dump and macro ptrs
- Log view / clear

## Requirements

- WebHID-compatible browser (Chrome / Edge, etc.)
- MediaPad firmware connected and running with WebHID support
- Target device must match:
  - Vendor ID: `0x1209`
  - Usage Page: `0xFF82`
  - Usage: `0x51`

## Usage

1. Open `ch552_mediaPad.html` in your browser.
2. Click "キーボードに接続" (Connect keyboard), then select the target device.
3. After connection, use the controls as needed:
   - "キーマップを読み出す": Read current key/encoder assignments
   - Click a key to change assignment (shown as pending on the UI)
   - "キーマップを書き込む": Apply pending keymap changes to the device
   - "マクロを編集する": Open macro editor and modify macro content
   - "マクロを書き込む": Save macro edits to EEPROM
4. If needed, use "デフォルトにリセット" (reset to defaults) or "ブートローダー開始" (start bootloader).

## Notes

- SW7 is treated as a fixed Fn key (`0xBC`) and cannot be changed in normal operation.
- Macro area size is 85 bytes (managed as a contiguous macro sequence).
- The per-macro-string spec limit is 50 bytes (validated on the UI side).
- Macro editor "Delay/Repeat" section:
  - Delay: `0xDC` (100ms), `0xDD` (500ms), `0xDE` (1sec)
  - Repeat: `0xDA` (SEGSTART), `0xDB` (GOTO_SEG)
- Firmware repeat behavior:
  - `GOTO_SEG` returns to the first `SEGSTART` found in the current macro string.
  - If no `SEGSTART` is found, execution returns to the start of the current macro string.
- If unsaved macro edits exist, reading macros again prompts for confirmation.
- On device disconnect, the app resets UI state and pending command state.

## Additional Info

- On connect, the app validates the firmware version (expected: `0x1234`).
- After starting the bootloader, the device may disconnect.
