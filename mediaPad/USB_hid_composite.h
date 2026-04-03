/**
 * USB_hid_composite.c hid composite device interface  of ch552 based macro keypad firmware.
 * Copyright (c) 2024 Takeshi Higasa, okiraku-camera.tokyo
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 * 
 * based on CH55xDuino USBHIDKeyboard.h
 */

#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"

#include "hid_keycode.h"

#ifdef __cplusplus
extern "C" {
#endif

void usbhid_init(void);
void send_key(uint16_t code, uint8_t pressed);
void send_modifiers(uint8_t mod, bool state) ;
void kbd_releaseAll(void);
uint8_t get_hid_ledstate();


extern __xdata uint8_t modifiers;


#ifdef __cplusplus
} // extern "C"
#endif
