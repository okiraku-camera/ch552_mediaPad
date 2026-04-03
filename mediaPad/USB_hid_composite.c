/**
 * USB_hid_composite.c hid composite device interface  of ch552 based keyboard firmware.
 * Copyright (c) 2024 Takeshi Higasa, okiraku-camera.tokyo
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 * 
 * based on CH55xDuino USBHIDKeyboard.c example.
 * 
 * ReportID 1: Keyboard report
 * ReportID 2: Mouse report.
 * ReportID 3: Media (Consumer) report.
 * ReportID 4: System report
 * ReportID 5: Raw data report for custom commands and data exchange.  * 
 * 
 */

#include "arduino.h"
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "USBconstant.h"
#include "USBhandler.h"
#include "USB_hid_composite.h"

#include "hid_keycode.h"

// Flag of whether upload pointer is busy
volatile __xdata uint8_t UpPoint1_Busy = 0;

// keyboard input report
// kbd_report_size < KEYBOARD_EPSIZE (defined in USBconstant.h)
__xdata uint8_t kbd_report[8];
__xdata uint8_t modifiers = 0;

uint8_t hid_ledstate = 0;

extern void delayMicroseconds(uint16_t us);

void usbhid_init() {
	USBDeviceCfg();         // Device mode configuration
	USBDeviceEndPointCfg(); // Endpoint configuration
	USBDeviceIntCfg();      // Interrupt configuration
	UEP0_T_LEN = 0;
	UEP1_T_LEN = 0; // Pre-use send length must be cleared
	UEP2_T_LEN = 0;
//
	for(__data uint8_t i = 0; i < sizeof(kbd_report); i++)
		kbd_report[i] = 0;
}

void USB_EP1_IN() {
	UEP1_T_LEN = 0;
	UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK; // Default NAK
	UpPoint1_Busy = 0;                                       // Clear busy flag
}

uint8_t USB_EP1_send(uint8_t id, const void* data, uint8_t len) {
	if (UsbConfig == 0)
		return 0;	
	if (len >= EPSIZE)
		return 0;
	__data uint16_t waitWriteCount = 0;
	waitWriteCount = 0;
	while (UpPoint1_Busy) { // wait for 1sec. or give up
		waitWriteCount++;
		delayMicroseconds(100);
		if (waitWriteCount >= 10000)	// watchdog reset will fire before returning.
			return 0;
	}

	Ep1Buffer[REPORT_IN_START_INDEX] = id;
	if (data) {
		const uint8_t* src = (const uint8_t*) data;
		for (uint8_t i = 1; i <= len; i++) // load data for upload
			Ep1Buffer[REPORT_IN_START_INDEX + i ] = *src++;
	}
	UEP1_T_LEN = len + 1;

	UpPoint1_Busy = 1;
	UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK; // upload data and respond ACK

	return 1;
}


//extern void via_raw_input(uint8_t* command);
extern uint8_t raw_input(uint8_t* data) ;

void USB_EP1_OUT() {
	if (U_TOG_OK) { 
		uint8_t request = Ep1Buffer[REPORT_OUT_START_INDEX];
		if (request == REPORT_ID_KBD) {	// normal keyboard LED
			hid_ledstate = Ep1Buffer[REPORT_OUT_START_INDEX + 1];
		} else if (request == REPORT_ID_RAW) { 
			P1 &= 0xfd;	//------
			for(int8_t i = 0; i <= RAW_DATA_SIZE; i++)
				Ep1Buffer[REPORT_IN_START_INDEX + i] = Ep1Buffer[REPORT_OUT_START_INDEX + i];	// copy contents. +1 to include data[31] (RAW_DATA_SIZE+1 total with report ID)
			int8_t bytes = raw_input(&Ep1Buffer[REPORT_IN_START_INDEX + 1]);
			// 常にすべてリターンする、でよいのか？ 無駄な時間
			USB_EP1_send(REPORT_ID_RAW, 0, RAW_DATA_SIZE);
		}
	}
}

uint8_t get_hid_ledstate() {
	return hid_ledstate;
}

// keyboard report.
void report_press(uint8_t k) {
	if (!k) {
		return;
	}
#if 0
	if (IS_MODIFIER_KEYCODE(k)) {
		uint8_t mod = 0;
		if (k >= 0xE0 && k <= 0xE3) {
			mod = (1 << (k - 0xE0));
		}
		if (mod)
			send_modifiers(mod, true);
		return;
	}
#endif

	for(__data uint8_t i = 2; i < sizeof(kbd_report); i++) {
		if (kbd_report[i] == k)
				return;
		if (kbd_report[i] == 0) {
			kbd_report[i] = k;
			kbd_report[0] = modifiers;	
			USB_EP1_send(REPORT_ID_KBD, kbd_report, sizeof(kbd_report));
			return;
		}
	}
}

void report_release(uint8_t k) {
	if (k != 0) {
		for(__data uint8_t i = 2; i < sizeof(kbd_report); i++) {
			if (kbd_report[i] == k) {
				kbd_report[i] = 0;
				break;
			}
		}
	}
	kbd_report[0] = modifiers;	
	USB_EP1_send(REPORT_ID_KBD, kbd_report, sizeof(kbd_report));
}

#define send_consumer 1
#define send_system  2
static uint8_t send_syscon = 0;	// system, consumer key pressed ?

#define MOUSE_BUTTON_POS	0
#define MOUSE_X_POS	1
#define MOUSE_Y_POS	2
#define MOUSE_Z_POS	3

#define MOUSE_DELTA_X	4
#define MOUSE_DELTA_Y	4
#define MOUSE_DELTA_Z	1

static void send_mouse_code(uint8_t code, bool pressed) {
	__xdata uint8_t mouse_report[4];
	mouse_report[MOUSE_X_POS] = 0;
	mouse_report[MOUSE_Y_POS] = 0;
	mouse_report[MOUSE_Z_POS] = 0;
	if (code == 0)	// force relase.
		mouse_report[MOUSE_BUTTON_POS] = 0;
	else if (code >= FN_MS_BTN1 && code <= FN_MS_BTN_END) {
		uint8_t btn = code - FN_MS_BTN1;
		if (pressed)
			mouse_report[MOUSE_BUTTON_POS] = (1 << btn);
		else
			mouse_report[MOUSE_BUTTON_POS] = 0; ~(1 << btn);
	} else if (pressed) {	// クリックのみ。長押しは考慮してない。
		switch(code) {
		case FN_MS_LEFT:
			mouse_report[MOUSE_X_POS] = -MOUSE_DELTA_X;
			break;
		case FN_MS_RIGHT:
			mouse_report[MOUSE_X_POS] = MOUSE_DELTA_X;
			break;
		case FN_MS_UP:
			mouse_report[MOUSE_Y_POS] = -MOUSE_DELTA_Y;
			break;
		case FN_MS_DOWN:
			mouse_report[MOUSE_Y_POS] = MOUSE_DELTA_Y;
			break;
		case FN_MS_WH_UP:
			mouse_report[MOUSE_Z_POS] = -MOUSE_DELTA_Z;
			break;
		case FN_MS_WH_DOWN:
			mouse_report[MOUSE_Z_POS] = MOUSE_DELTA_Z;
			break;
		}
	}
	USB_EP1_send(REPORT_ID_MOUSE, &mouse_report, sizeof(mouse_report));
}


static void send_system_code(uint16_t code, bool pressed) {
	uint16_t data = pressed ? bit((code - FN_SYSTEM_CODE_START)) : 0;
	USB_EP1_send(REPORT_ID_SYSTEM, &data, sizeof(data));
	send_syscon = pressed ? (send_syscon | send_system) : (send_syscon & ~send_system);
}

static void send_media_code(uint8_t code, bool pressed) {
	uint16_t data = 0;
	if (pressed) {
		switch(code) {
		case FN_MEDIA_VOL_UP:
			data = 0x00e9;
			break;
		case FN_MEDIA_VOL_DOWN:
			data = 0x00ea;
			break;
		case FN_MEDIA_MUTE:
			data = 0x00e2;
			break;
		case FN_MEDIA_PLAY_PAUSE:
			data = 0x00cd;
			break;
		case FN_MEDIA_SCAN_NEXT:
			data = 0x00b5;
			break;
		case FN_MEDIA_SCAN_PREV:
			data = 0x00b6;
			break;
		case FN_MEDIA_STOP:
			data = 0x00b7;
			break;
		default:
			return;
		}
	}
	USB_EP1_send(REPORT_ID_CONSUMER, &data, sizeof(data));
	send_syscon = pressed ? (send_syscon | send_consumer) : (send_syscon & ~send_consumer);
}

void kbd_releaseAll(void) {
	for(__data uint8_t i = 0; i < sizeof(kbd_report); i++)
		kbd_report[i] = 0;
	modifiers = 0;
	USB_EP1_send(REPORT_ID_KBD, kbd_report, sizeof(kbd_report));
}

void mouse_releaseAll() {
	send_mouse_code(0, false);
}

void send_modifiers(uint8_t mod, bool state) {
	__data uint8_t rep[sizeof(kbd_report)];
	for(__data uint8_t i = 0; i < sizeof(kbd_report); i	++)
		rep[i] = 0;

	if (state) {
		modifiers |= mod;
	} else {
		//modifiers &= ~mod;
		modifiers = 0;
	}
	rep[0] = modifiers;
	USB_EP1_send(REPORT_ID_KBD, rep, sizeof(rep));
}	

void send_key(uint16_t code, uint8_t pressed) {
	if (code == 0) {	// force all off.
		if (send_syscon & send_consumer)
			send_media_code(0, false);
		if (send_syscon & send_system)
			send_system_code(0, false);
		send_syscon = 0;
		mouse_releaseAll();
		kbd_releaseAll();
		return;
	}
	if (IS_SYSTEM_KEYCODE(code))
		send_system_code(code, pressed);
	else if (IS_CONSUMER_KEYCODE(code))
		send_media_code(code, pressed);
	else if (IS_MOUSE_CODE(code)) 
		send_mouse_code(code, pressed);
	else {	// normal keys or unknown.
		code &= 0xff;
		if (IS_BASIC_KEYCODE(code) || IS_MODIFIER_KEYCODE(code)) {	
			if (pressed)
				report_press(code);
			else
				report_release(code);	
		}
	}
}
