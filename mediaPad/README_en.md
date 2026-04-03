# mediaPad firmware

This directory contains the mediaPad firmware (Arduino sketch) for CH552G.

## Overview

mediaPad is a USB-connected macropad.  
The firmware handles key scanning, rotary encoder input, USB HID transmission, and settings storage (EEPROM).

## Main Behavior

- Scans a 2x4 key matrix (8 keys) and sends assigned keycodes
- Sends keycodes based on rotary encoder direction (CW / CCW)
- Uses a 2-layer keymap stored in EEPROM
- Writes the default keymap to EEPROM on first boot or reset
- Stores macro data in EEPROM and sends macro sequences from assigned keys
- Receives configuration commands via WebHID to update keymaps and macros

## Key Files

- `mediaPad.ino`: Main logic (input scan, key send, layer/macro control)
- `USB_hid_composite.c/.h`: USB HID send/receive processing
- `hid_raw_request.c`: WebHID configuration command handling (read/write requests)
- `nvm_keymap.c/.h`: EEPROM keymap/macro storage processing
- `r_encoder.c/.h`: Rotary encoder input processing
- `serial_led.c/.h`: LED control
- `hardware_config.h`: Constants such as key count and EEPROM memory map

## Build and Flash

1. Install Arduino IDE and add the board package for CH552G.
2. Open `mediaPad.ino` and set the board to CH552G.
3. In build options, set USB Settings to **USER CODE w/ 148B USB ram** (required).
4. Build and flash as in a normal Arduino workflow.

### Meaning of the USB Settings Option

- **USER CODE**: Uses user-implemented USB handling instead of a default USB class. This is required for this project's custom HID behavior (composite HID + WebHID communication).
- **148B USB ram**: Allocates 148 bytes of CH552 USB-dedicated RAM. This is used for endpoint buffer allocation and is required for stable USB communication.

## Note

Use [html/ch552_mediaPad.html](../html/ch552_mediaPad.html) in the parent directory as the Web configuration tool.  
It allows reading/writing keymaps and macros, and resetting to defaults.
