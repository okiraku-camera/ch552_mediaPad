/**
 * serial_led.c serial-led (also called addressable-led) control for ch55x.
 * Copyright (c) 2024 Takeshi Higasa, okiraku-camera.tokyo
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 *
 * Tested on SK6812MINI-HS SK6812-B chains.
 * 
 */
#include "serial_led.h"

#define SLED_COUNT 5
#define BYTE_COUNT (SLED_COUNT * 3)

#define _SLED_PIN _P1_1
#define SLED_PIN 11

#define SLED_VALUE_MAX	240
#define SLED_VALUE_INIT	120
#define SLED_VALUE_DELTA	4


#if F_CPU == 24000000 
	#define T0H_DELAY \
		nop \
		nop \
		nop \
		nop 
	#define T1H_DELAY \
		nop \
		nop \
		nop \
		nop \
		nop \
		nop \
		nop 
	#define TL_DELAY \
		nop \
		nop \
		nop \
		nop \
		nop \
		nop \
		nop 
#elif	F_CPU == 16000000
	#define T0H_DELAY \
		nop \
		nop \
		nop 
	#define T1H_DELAY \
		nop \
		nop \
		nop 
	#define TL_DELAY \
		nop \
		nop \
		nop \
		nop 
#elif F_CPU == 12000000
	#define T0H_DELAY \
		nop \
		nop 
	#define T1H_DELAY \
		nop 
	#define TL_DELAY \
		nop \
		nop 
#else
	#error F_CPU must be 24M or 16M or 12M.
#endif

__xdata uint8_t led_data[BYTE_COUNT];

void set_color_data(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
	uint8_t i = index + index + index;
	led_data[i++] = g;
	led_data[i++] = r;
	led_data[i] = b;
}

void send_data(uint8_t* data ) {
__asm
		.even
		clr _EA			// disable interrupt.
		mov r1, #BYTE_COUNT	// bytes to send.
	0001$:
		movx	 a,@dptr
		inc dptr       
		mov r2, #8	// 8 bit to send.
	0002$:
		rlc a					// 1 CLK. MSB to carry flag.
		setb _SLED_PIN	// 2 CLK
		T0H_DELAY
		mov _SLED_PIN, C	// 2 CLK
		T1H_DELAY
		clr _SLED_PIN		// 2 CLK
		TL_DELAY
		djnz r2,	0002$	// 2  / 4 or 5 or 6.
		djnz r1,	0001$
	// 2  / 4 or 5 or 6.
		setb _EA		// enable interrupt.
__endasm;
data;
}

static uint8_t sled_value = 0;

void sled_off() {
	send_rgb_to_all(0, 0, 0);
	sled_value = 0;
}

uint8_t get_sled_value() { return sled_value; }

void send_rgb_to_all(uint8_t r, uint8_t g, uint8_t b ) {
	for(uint8_t i = 0; i < SLED_COUNT; i++)
		set_color_data(i, r, g, b);
	send_data(led_data);
}

void serial_led_init() {
	pinMode(SLED_PIN, OUTPUT); 
	digitalWrite(SLED_PIN, 0);
	delay(1);
	sled_off();
} 

uint8_t map_by_value(uint8_t x) {
	return (uint16_t)(x * sled_value) / 255;
}

static uint8_t cur_r = 0;
static uint8_t cur_g = 0;
static uint8_t cur_b = 0;

void change_led_value(int8_t delta) {
	__data int16_t value = sled_value;
	value += delta;
	if (value < 1)
		value = 1;
	else if (value > 255)
		value = 255;
	sled_value = value & 0xff;		
}

void sled_max() { sled_value = SLED_VALUE_MAX; }
void sled_dim()  { 
	change_led_value(-SLED_VALUE_DELTA); 
	if (sled_value < 2) {
		sled_off();
		return;
	}
	uint8_t r = map_by_value(cur_r);
	uint8_t b = map_by_value(cur_b);
	uint8_t g = map_by_value(cur_g);
	send_rgb_to_all(r, g, b);
}

// hue 0..359
void hue_to_led(int16_t hue) {
	if (sled_value == 0)
		sled_value = SLED_VALUE_INIT;

	if (hue >= 360 || hue < 0)
		hue = 0;

	if (hue < 60) {
		cur_r = 255;
		cur_g = (hue * 255) / 60;
		cur_b = 0;
	} else if (hue < 120) {
		cur_r = ((120 - hue) * 255) / 60;
		cur_g = 255;
		cur_b = 0;
	} else if (hue < 180) {
		cur_r = 0;
		cur_g = 255;
		cur_b = (hue - 120) * 255 / 60;
	} else if (hue < 240) {
		cur_r = 0;
		cur_g = ((240 - hue) * 255) / 60;
		cur_b = 255;
	} else if (hue < 300) {
		cur_r = (hue - 240) * 255 / 60;
		cur_g = 0;
		cur_b = 255;
	} else {
		cur_r = 255;
		cur_g = 0;
		cur_b = ((360 - hue) * 255) / 60;
	}
	uint8_t r = map_by_value(cur_r);
	uint8_t b = map_by_value(cur_b);
	uint8_t g = map_by_value(cur_g);
	send_rgb_to_all(r, g, b);
}


