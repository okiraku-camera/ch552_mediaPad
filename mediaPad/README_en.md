# mediaPad firmware

Japanese version is here: [README.md](README.md)

This directory contains the mediaPad firmware (Arduino sketch) for CH552G.

## Overview

mediaPad is a **programmable media pad / macro pad** based on CH552G.  
It handles key scanning, rotary encoder input, USB HID transmission, and persistent storage of keymaps/macros, and its settings can be updated from a WebHID-based Web UI.

The target hardware for this firmware is **ch552_mediaPad**.

## Main Features

- **8 keys + rotary encoder input**: handles a 2x4 key matrix and encoder rotation
- **2-layer keymap**: uses a two-layer keymap stored in EEPROM
- **Encoder assignment**: CW / CCW keycodes can be configured per layer
- **Macro support**: stores up to 6 macros and sends them from assigned keys
- **WebUI configuration**: keymaps, encoder assignments, and macros can be edited from the browser via WebHID
- **Persistent storage**: settings are stored in NVM (built-in EEPROM)

## Software Structure

### Hardware Layer
| File | Role |
|------|------|
| `mediaPad.ino` | Main sketch: input scan, key send, layer/macro control |
| `r_encoder.c/.h` | Rotary encoder input processing |
| `serial_led.c/.h` | LED control |

### Layer and Keymap Management
| File | Role |
|------|------|
| `nvm_keymap.c/.h` | EEPROM keymap/macro storage processing |
| `hardware_config.h` | Constants such as key count, layer count, and EEPROM layout |

### USB HID Communication
| File | Role |
|------|------|
| `USB_hid_composite.c/.h` | USB HID composite device implementation |
| `hid_raw_request.c` | WebHID configuration request handling (read/write commands) |

### PC-side Web Application
| File | Role |
|------|------|
| `html/ch552_mediaPad.html` | Web application UI |
| `html/ch552_mediaPad.js` | WebHID communication and UI logic |
| `html/style.css` | Styling |

## Operation Modes

### Layer Switching
- The firmware uses a 2-layer keymap
- SW7 is treated as a fixed Fn key and is not a normal remappable key
- Fn operation switches the current layer

### Encoder Behavior
- The device uses one rotary encoder
- Separate keycodes can be assigned to CW and CCW for each layer
- The default use case assumes media-control or scroll-related keycodes

### Macro Behavior
- Pressing a macro key sends the registered macro sequence
- If another key is pressed during macro execution, the running macro stops
- Macros can include delay codes and loop-control codes

## Usage

### Quick Start
1. Open `html/ch552_mediaPad.html` in a browser.
2. Click "Connect to Keyboard" and select the device.
3. Edit the keymap, encoder assignments, and macros as needed.
4. Save changes to the device with "Write keymap" and "Write macro".

### Detailed WebUI Guide

Detailed WebUI steps are documented in the following files.

- Japanese: `html/README.md`
- English: `html/README_en.md`

## Build

The following software is required to build this firmware.

- Visual Studio Code + vscode-arduino extension
- arduino-cli
- CH55xDuino
- `ch552_keyPad_Library` under Arduino libraries is required.

### Device Split by arduino.json

This project separates device-specific build settings using `.vscode/arduino.json` under each sketch directory.

- `macro_numpad/.vscode/arduino.json`:
	- `sketch`: `macro_numpad.ino`
	- `buildPreferences`: `-DBOARD_NAME=CH552_MACRO_NUMPAD -I../common`
- `mediaPad/.vscode/arduino.json`:
	- `sketch`: `mediaPad.ino`
	- `buildPreferences`: `-DBOARD_NAME=CH552_MEDIAPAD -I../common`

With this setup, shared modules in `software/common` are reused while hardware-dependent data is separated by the `BOARD_NAME` macro. See the `.vscode/arduino.json` files in the source tree for the concrete settings.

The `.vscode/arduino.json` file looks like this:

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

The vscode-arduino extension uses arduino-cli internally and builds with these settings.

### Board Settings

Select the following option in board settings.

- USB Settings: `USER CODE w/ 148B USB ram`

This setting is required to allocate the USB RAM needed for HID endpoints.

## Technical Details

### NVM layout (128 bytes)
- **Bytes 0-1**: keymap valid flag (`0xAA55`)
- **Bytes 2-33**: key keymap for 2 layers (8 keys x 2 bytes x 2 layers)
- **Bytes 34-41**: rotary encoder assignments (1 encoder x CW/CCW x 2 layers x 2 bytes)
- **Bytes 42-126**: macro string data
- **Byte 127**: macro area terminator (reserved)

### Macro Features
- Supports **up to 6 macros**
- **Macro storage size is 85 bytes**
- UI limit for one macro string is **50 bytes**
- Each macro is stored as a zero-terminated continuous sequence

### Macro Control Codes
- **Delay codes**:
	- `0xDC` = 100 ms
	- `0xDD` = 500 ms
	- `0xDE` = 1 sec
- **Loop control codes**:
	- `0xDA` = SEGSTART
	- `0xDB` = GOTO_SEG

### Loop Behavior
- `GOTO_SEG` jumps back to the first `SEGSTART` found in the current macro
- If no `SEGSTART` exists, it jumps back to the beginning of the macro

## Troubleshooting

### Cannot connect to the device
- Check whether your browser supports WebHID (Chrome and Edge recommended)
- Check whether the device is connected in a WebHID-capable state
- Check USB access permissions in the browser

### Keymap or encoder settings are not applied
- Make sure you saved changes with "Write keymap"
- Pending changes are not applied by read operations alone
- Try running "Read keymap" once and then editing again to help isolate the issue

### Macros do not behave as expected
- Check whether the macro slot and assigned key are correct
- Check whether the macro storage limit has been exceeded
- Check whether delay codes and loop-control codes are arranged as intended

## Notes

- Use `html/ch552_mediaPad.html` as the Web configuration tool
- The device version is checked on connection (expected value: `0x1234`)
- Starting the bootloader may temporarily disconnect the device
