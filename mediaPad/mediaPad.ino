/**
 * mediaPad.ino main sketch of mediapad program using ch552g.
 * Copyright (c)  Takeshi Higasa, okiraku-camera.tokyo
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 * 
 */

#include "USB_hid_composite.h"
#include "hid_keycode.h"
#include "ch552_watchdog.h"
#include "nvm_keymap.h"	
#include "serial_led.h"
#include "hardware_config.h"	// for hardware dependent constants.

#define numlock_led  11
const uint8_t cols[] = {30, 31};
const uint8_t rows[] = {14, 15, 16, 17};
const uint8_t row_masks[] = {0x10, 0x20, 0x40, 0x80};

#define KB_LED_PIN	32
#define KB_LED_OFF 	1	// high to off.
#define KB_LED_ON 	0

void led_off() {
	sled_off();
	digitalWrite(KB_LED_PIN, KB_LED_OFF);
}


// Initial layout .
// This layout is written to NVM on first boot and reset command. User can change the layout by writing to NVM.
const uint16_t intial_layout[NUM_KEYS] = {
	HID_KEYPAD_0,
	HID_KEYPAD_1,
	HID_KEYPAD_2,
	HID_KEYPAD_3,
	HID_KEYPAD_4,
	HID_KEYPAD_5,
	HID_KEYPAD_6,
	HID_KEYPAD_7
};

const uint16_t intial_re_layout[RE_COUNT * 2] = {
	FN_MEDIA_VOL_UP, 
	FN_MEDIA_VOL_DOWN,	// RE0 layer0, CW, CCW
	FN_MS_WH_UP,
	FN_MS_WH_DOWN		// RE0 layer1, CW, CCW
};

void build_initial_layout() {
	// build initial layout. this is used when nvm is not valid.
	for(int8_t i = 0; i < NUM_KEYS ; i++) {	
		uint16_t code = intial_layout[i];
		nvm_write_word(KEYMAP_START + (i << 1), code);				// レイヤー0はテンキー
		nvm_write_word(KEYMAP_START + NUM_KEYS * 2 + (i << 1), 0);	// レイヤー1はすべて0
	}
	for(int8_t i = 0; i < RE_COUNT * 2; i++) {	
		uint16_t code = intial_re_layout[i];
		nvm_write_word(RE_KEYMAP_START + (i << 1), code);				// レイヤー0
		code = intial_re_layout[i + 2];
		nvm_write_word(RE_KEYMAP_START + RE_COUNT * 4 + (i << 1), 0);	// レイヤー1
	}

}

int8_t current_layer = 0;	// 0 or 1. this is used for macropad layer switching.

void write_code_callback(uint8_t sw, uint8_t layer, uint16_t code) {
	sw;
	code;
	layer;
	// this is a callback function called when key code is written to NVM.
	// user can implement custom processing here when key code is updated, such as sending a notification to host, etc.
}


// called from USBhandler
void USBStartSuspend() {
	led_off();
}

uint8_t active_macro = 0;	// 0 means no active macro. 1～MACRO_KEY_COUNT means active macro index. this is set when macro key is pressed, and reset when macro code is sent.
uint8_t macro_index = 0;	// Index tracking the current position within the macro sequence referenced by activa_macro.
int8_t macro_delay_count = 0;	// Counter for tracking delay steps in macro execution, used when processing delay codes within a macro sequence.

typedef struct {
	uint16_t code;
	uint8_t pressed;
} queued_key_t;

#define KEY_SEND_QUEUE_BYTES 10
static uint8_t key_send_queue[KEY_SEND_QUEUE_BYTES];
static uint8_t key_send_queue_head = 0;
static uint8_t key_send_queue_tail = 0;
static uint8_t key_send_queue_used = 0;

static void key_send_queue_put_byte(uint8_t value) {
	key_send_queue[key_send_queue_tail] = value;
	key_send_queue_tail = (key_send_queue_tail + 1) % KEY_SEND_QUEUE_BYTES;
}

static uint8_t key_send_queue_get_byte() {
	uint8_t value = key_send_queue[key_send_queue_head];
	key_send_queue_head = (key_send_queue_head + 1) % KEY_SEND_QUEUE_BYTES;
	return value;
}

bool put_key_send_queue(uint16_t code, uint8_t pressed) {
	if (key_send_queue_used + sizeof(queued_key_t) > KEY_SEND_QUEUE_BYTES)
		return false;
	key_send_queue_put_byte((uint8_t)(code & 0xFF));
	key_send_queue_put_byte((uint8_t)((code >> 8) & 0xFF));
	key_send_queue_put_byte(pressed ? 1 : 0);
	key_send_queue_used += sizeof(queued_key_t);
	return true;
}

bool get_key_send_queue(queued_key_t* out) {
	if (!out)
		return false;
	if (key_send_queue_used < sizeof(queued_key_t))
		return false;
	uint8_t lo = key_send_queue_get_byte();
	uint8_t hi = key_send_queue_get_byte();
	out->code = (uint16_t)lo | ((uint16_t)hi << 8);
	out->pressed = key_send_queue_get_byte();
	key_send_queue_used -= sizeof(queued_key_t);
	return true;
}

static bool should_run_periodic(unsigned long* last_time, unsigned long interval_ms) {
	unsigned long now = millis();
	if (*last_time == 0) {
		*last_time = now;
		return false;
	}
	if ((unsigned long)(now - *last_time) < interval_ms)
		return false;
	*last_time = now;
	return true;
}

void process_send_key_queue() {
	static unsigned long last_time = 0;
	if (!should_run_periodic(&last_time, 10))
		return;

	queued_key_t item;
	if (get_key_send_queue(&item)) {
		send_key(item.code, item.pressed);
	}
}

void key_event(uint8_t switch_num, uint8_t state) {
	if (switch_num < 1 || switch_num > NUM_KEYS )
		return;

	if (active_macro) {
		if (state) {
			macro_index = 0;
			macro_delay_count = 0;
			active_macro = 0;
			kbd_releaseAll();
		}
		return;
	}	
	// get a assigned code of switch_num.
	uint16_t hidcode = read_nvm_code(switch_num - 1, current_layer);
	// 
	if (hidcode != 0) {
		uint8_t code = (uint8_t)hidcode & 0xff;
		uint8_t mod = (hidcode >> 8) & 0xff; 
		if ((code == HID_X_FN1) && state) {	// layer switch key. this is not sent to host, but used for layer switching.
			current_layer ^= 1;
			return;
		}
		if (IS_MACRO_KEYCODE(code)) {
			if (state) {
				active_macro = hidcode - MACRO_KEYCODE_BASE;
				if (macro_ptrs[active_macro] == 0) {	// no macro code.
					active_macro = 0;
					return;
				}
				active_macro++;
				macro_index = 0;
				macro_delay_count = 0;
			}
		} else {
			if (!state)
				send_key(code, 0);
			if (mod) {
				send_modifiers(mod, state);
				delay(20);
			}
			if (state)
			send_key(code, 1);
		}
	}
}

static uint8_t last_stable = 0;
static uint8_t prev_states = 0;
void scan() {
	__data uint8_t switch_states = 0;
	for(uint8_t row = 0; row < sizeof(rows); row++) {
		P1 &= ~row_masks[row];		// select a row.
		delayMicroseconds(25);
		switch_states |= ((~P3 & 3) << (row << 1));
		P1 |= row_masks[row];	
		delayMicroseconds(25);
	}
	// check if current result and previous result is match.
	bool f = (prev_states != switch_states);
	prev_states = switch_states;
	if (f) 	return;	// no match. may be bounced.

	uint8_t changes = switch_states ^ last_stable;
	last_stable = switch_states;
	if (changes) {
		uint8_t mask = 1;
		for (uint8_t i = 0; i < 8; i++, mask <<=1 )	
			if (changes & mask) {	// check if switch states are changed or not.
				uint8_t sw = i + 1;
				key_event(sw, (switch_states & mask) ? 1 : 0);	
			}
	}
}

// boot loader entry address is defined in ch5xx.h
void enter_bootloader_mode() {
#if defined(BOOT_LOAD_ADDR)
	led_off();
	USB_CTRL = 0;
	EA = 0; // Disabling all interrupts is required.
	TMOD = 0;
	delay(10);
	__asm__("lcall #0x3800"); // Jump to bootloader code
	for(;;) {}
#endif
}

// type_macro_strings() is called repeatedly from loop() by 10msec.  
// When a modifier keycode appears in the macro, send the corresponding modifier-on event.
// When the macro reaches its end, if any modifiers were turned on, send modifier-off events for all of them.
// Modifier-off events at the end are sent regardless of the physical modifier state.
void type_macro_strings() {
	static uint8_t mod = 0;
	static uint8_t seg_start_index = 0xff;	// this is used for tracking the start index of a segment when processing segment start and goto codes in macro. 0xff means no segment start encountered yet.	

	if (macro_delay_count > 0) {
		macro_delay_count--;
		return ;
	}

	if (macro_index == 0)
		seg_start_index = 0xff;

	uint16_t macro_base = macro_ptrs[active_macro - 1];
	if (macro_base == 0) {	// no macro code.
		active_macro = 0;
		macro_delay_count = 0;
		seg_start_index = 0xff;
		return ;
	}
	uint8_t code = eeprom_read_byte(macro_base + macro_index);
	if (code == 0) {	// end of macro code.
		active_macro = 0;
		macro_index = 0;
		macro_delay_count = 0;
		mod = 0;
		seg_start_index = 0xff;
		kbd_releaseAll();
		return ;
	}

	if (code == HID_M_SEGSTART) {
		if (seg_start_index == 0xff) {
			seg_start_index = macro_index;
		}
		macro_index++;
		return ;
	}

	if (code == HID_M_GOTO_SEG) {
		macro_index = (seg_start_index == 0xff) ? 0 : seg_start_index;
		return ;
	}

	if (IS_MACRO_DELAY_CODE(code)) {
		switch (code) {
		case HID_M_DELAY100:
			macro_delay_count = 10;	// set delay count for 100ms.
			break;
		case HID_M_DELAY500:
			macro_delay_count = 50;	// set delay count for 500ms.
			break;
		case HID_M_DELAY1000:
			macro_delay_count = 100;	// set delay count for 1000ms.
			break;
		default:
			macro_delay_count = 0;
		}
		macro_index++;
		return ;
	}

	if (IS_MODIFIER_KEYCODE(code)) { 
		mod |= (1 << ((code - HID_MODIFIERS) & 0x7));	// 0～7 for left mods.
		if ((mod & modifiers) & mod) {	// if some modifiers are already on, send the new modifiers state.	
			send_modifiers(mod, false);
		} else {
			send_modifiers(mod, true);
		}
		delay(20);
	} else {
		send_key(code, 1);
		delay(10);
		send_key(code, 0);
	}
		macro_index++;
	return ;
}

void keyscan() {
	static unsigned long last_time = 0;
	if (!should_run_periodic(&last_time, 20))
		return;
	if (active_macro)
		type_macro_strings();
	scan();
	// ついでにLEDも更新。レイヤー0のときは消灯、レイヤー1のときは点灯。
	digitalWrite(KB_LED_PIN, current_layer == 0 ? KB_LED_OFF : KB_LED_ON);
}


// 所定の時間が経過したら暗くし、消灯する。
unsigned long bg_color_time = 0;
unsigned long sled_dim_time = 0;
void sled_task() {
	if (bg_color_time == 0)
		return;
	unsigned long now = millis();
	if (now - bg_color_time < 1000)
		return;
	// 1秒経過後、50msごとに段階的に暗くする。
	if (sled_dim_time == 0 || now - sled_dim_time >= 50) {
		sled_dim();
		sled_dim_time = now;
		if (get_sled_value() < 1) {
			sled_off();
			bg_color_time = 0;
			sled_dim_time = 0;
		}
	}
}

// Rotate color wheel
// turn off bg-leds after 10 seconds of lighting. 
#define DIR_CW	0
#define DIR_CCW	1

// bg_color() is called from encoder_task() when the encoder is turned, 
//  and also can be called with extra=true to trigger bg color change regardless of encoder rotation.
void bg_color(uint8_t dir, bool extra) {
	static int16_t hue = 0;	
	bg_color_time = millis();
	sled_dim_time = 0;
	if (extra)
		dir = DIR_CW;
	if (dir == DIR_CW)
		hue = hue + 2;
	else 
		hue = hue - 2;
	if (hue < 0)
		hue = 359;
	else if (hue > 359)
		hue = 0;	
	hue_to_led(hue);
}

#include "r_encoder.h"
void encoder_task() {
	static int16_t last_counter = 0;
	static unsigned long last_time = 0;
	if (!should_run_periodic(&last_time, 2))
		return;
	int16_t delta = get_encoder_count() - last_counter;
	if (delta == 0)
		return;
	uint8_t dir = delta < 0 ? DIR_CCW : DIR_CW;
	uint16_t code = get_re_keycode(0, current_layer, dir);
	put_key_send_queue(code, 1);
	put_key_send_queue(code, 0);
	last_counter = get_encoder_count();
	bg_color(dir, false);
} 

// Clarity is important
void setup() {
	usbhid_init();
	pinMode(KB_LED_PIN, OUTPUT);
	serial_led_init();
	led_off();
	encoder_init();

	for(uint8_t i = 0; i < sizeof(cols); i++)
		pinMode(cols[i], INPUT_PULLUP);
	for(uint8_t i = 0; i < sizeof(rows); i++) {
		pinMode(rows[i], OUTPUT);
		digitalWrite(rows[i], 1);
	}
	init_nvm_keymap();
	wdt_enable(WDT_TIMEOUT);
}

void loop() {
	wdt_update();
	keyscan();
	encoder_task();
	process_send_key_queue();
	sled_task();
	delay(2);
}
