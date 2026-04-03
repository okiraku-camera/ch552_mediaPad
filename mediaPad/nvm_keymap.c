#include "Arduino.h"
#include "hardware_config.h"
#include "hid_keycode.h"

// nvm_keymap.c for mediaPad.ino. (ch552g)
// Copyright (c)  Takeshi Higasa, okiraku-camera.tokyo
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
// 
// This file handles reading and writing keymap and macro data to NVM, 
//  as well as providing functions to read/write individual key codes and macro code strings.	

// magic number to check if nvm has valid keymap data. 
// if these bytes are not written at NVM_START, it is considered that uninitialized or corrupted.
#define NVM_KEYMAP_VALID	0xBEEF

// extrcted ptr array for macro code strings. 
__xdata uint8_t macro_ptrs[MACRO_KEY_COUNT];

extern uint8_t eeprom_read_byte(__data uint8_t addr);
extern void eeprom_write_byte(__data uint8_t addr, __xdata uint8_t val);


extern void build_initial_layout();	// in main sketch.
extern void write_code_callback(uint8_t sw, uint8_t layer, uint16_t code);


// verify_write checks if the value at the given address is already the desired value, and only writes if it is different. 
// This is to minimize unnecessary write cycles to NVM, which has a limited lifespan.
void verify_write(uint8_t addr, uint8_t val) {
	uint8_t read_val = eeprom_read_byte(addr);
	if (read_val != val)
		eeprom_write_byte(addr, val);
}

// LSB first storage of 16-bit data in NVM.	
void nvm_write_word(uint8_t addr, uint16_t data) {
	verify_write(addr, data);
	verify_write(addr + 1, data >> 8);
}

uint16_t nvm_read_word(uint8_t addr) {
	uint16_t code = (uint16_t)(eeprom_read_byte(addr + 1) << 8) + eeprom_read_byte(addr);
	return code;
}

const bool is_nvm_valid() {
	return ((nvm_read_word(EEPROM_START) == NVM_KEYMAP_VALID));
}

void reset_nvm() {
	build_initial_layout();
	for(int8_t i = 0; i < MACRO_SIZE; i++)	// reset macro code.
		eeprom_write_byte(MACRO_START + i, 0);	
	for(int8_t i = 0; i < MACRO_KEY_COUNT; i++)	// reset macro code pointers.
		macro_ptrs[i] = 0;	

	nvm_write_word(EEPROM_START, NVM_KEYMAP_VALID);
}

uint16_t read_nvm_code(uint8_t sw, uint8_t layer) {
	uint8_t index = layer ? KEYMAP_START + NUM_KEYS * 2 : KEYMAP_START;
	sw <<= 1;
	return nvm_read_word(index + sw);
}

void write_nvm_code(uint8_t sw, uint8_t layer, uint16_t code) {
	uint8_t index = layer ? KEYMAP_START + NUM_KEYS * 2 : KEYMAP_START;
	write_code_callback(sw, layer, code);
	sw <<= 1;
	nvm_write_word(index + sw, code);
}

/**
 * Specify the starting switch number, the number of switches, and the layer number.
 * The buffer will be filled with the key codes for the specified number of switches. 
 * The return value is the actual number of switches read	( not read bytes ).
 */
int8_t read_keys(int8_t start_sw, int8_t count, int8_t layer, uint16_t* buffer) {
	int8_t i;
	for(i = 0; i < count; i++) {
		if (i >= NUM_KEYS)
			break;
		buffer[i] = read_nvm_code(start_sw + i, layer);
	}
	return i;
}

int8_t write_keys(int8_t start_sw, int8_t count, int8_t layer, uint16_t* buffer) {
	int8_t i;
	for(i = 0; i < count; i++) {
		if (i >= NUM_KEYS)
			break;
		write_nvm_code(start_sw + i, layer, buffer[i]);
	}
	return i;
}

/**
 * Build macro index from stored macro code strings.
 * This function is called at program start and when the macro code strings are updated.
 */
void build_macro_index() {
    uint8_t addr = MACRO_START;
    int8_t total_strings = 0;

	for(int8_t i = 0; i < MACRO_KEY_COUNT; i++)	// reset macro code pointers.
		macro_ptrs[i] = 0;	

    if (eeprom_read_byte(addr) == '\0')
		return;

	macro_ptrs[total_strings++] = addr;
    while (total_strings < MACRO_KEY_COUNT && addr < MACRO_START + MACRO_SIZE) {
        uint8_t current = eeprom_read_byte(addr);
        if (current == '\0') {
            uint8_t next = eeprom_read_byte(addr + 1);
            if (next == '\0') 	// \0\0 end.
                break;
             else 
                macro_ptrs[total_strings++] = addr + 1;
        }
        addr++;
    }
}

// 
// Copy specified number of bytes from macro string area to buffer.
//
int8_t read_macros(int8_t start_index, int8_t bytes, char* buffer) {
	int8_t i;
	for(i = 0; i < bytes; i++) {
		if (i >= MACRO_SIZE)
			break;
		buffer[i] = eeprom_read_byte(MACRO_START + start_index + i);
	}
	return i;
}
// 
uint8_t write_macros(int8_t start_index, int8_t bytes, uint8_t* buffer) {
	int8_t i;
	for(i = 0; i < min(bytes, MACRO_SIZE); i++)
		verify_write(MACRO_START + start_index + i, buffer[i]);
	verify_write(MACRO_END, 0);	// write terminal 0.
	build_macro_index();
	return bytes;
}

// write RE keycode block.
// Specify the starting RE number, the number of REs, and the layer number.	
// The buffer should contain the key codes for the specified number of REs.
// The return value is the actual number of REs written.	
int8_t write_re_keys(int8_t reno, int8_t layer, int8_t count, uint16_t* buffer) {
	if (RE_COUNT <= 0 || reno >= RE_COUNT || layer >= NUM_LAYERS || count <= 0)
		return 0;
	int8_t i;
	uint8_t addr = layer ? RE_KEYMAP_START + RE_COUNT * 4 : RE_KEYMAP_START;
	addr += (reno << 2); // 4 bytes for each RE (CW, CCW).

	for (i = 0; i < count; i++) {
		if (reno + i >= RE_COUNT)
			break;
		// 1 encoder = 2 words: CW then CCW
		uint8_t base = addr + (i << 2);
		nvm_write_word(base, buffer[i * 2]);
		nvm_write_word(base + 2, buffer[i * 2 + 1]);
	}
	return i;
}

/**
 * read RE keycode block.
 * Specify the starting RE number, the number of REs, and the layer number. 
 * The buffer will be filled with the key codes for the specified number of REs. T
 * he return value is the actual number of REs read.
 * 4 bytes for each RE (CW, CCW). CW and CCW are stored in the order of CW first, then CCW.
 */
int8_t read_re_keys(int8_t start_reno, int8_t layer,  int8_t count, uint16_t* buffer) {
	if (RE_COUNT <= 0 || start_reno >= RE_COUNT || layer >= NUM_LAYERS || count <= 0)
		return 0;
	int8_t i;
	int8_t addr = layer ? RE_KEYMAP_START + RE_COUNT * 4 : RE_KEYMAP_START;
	addr += (start_reno << 2);	// 4 bytes for each RE (CW, CCW).
	for(i = 0; i < count; i++) {
		if (start_reno + i >= RE_COUNT)
			break;
		// 1 encoder = 2 words: CW then CCW
		uint8_t base = addr + (i << 2);
		buffer[i * 2] = nvm_read_word(base);
		buffer[i * 2 + 1] = nvm_read_word(base + 2);
	}
	return i;
}

uint16_t get_re_keycode(int8_t reno, int8_t layer, uint8_t direction) {
	if (RE_COUNT <= 0 || reno >= RE_COUNT || layer >= NUM_LAYERS)
		return 0;
	uint8_t addr = layer ? RE_KEYMAP_START + RE_COUNT * 4 : RE_KEYMAP_START;
	reno <<= 2;	// 4 bytes for each RE (CW, CCW).
	if (direction == 0)	// CW
		return nvm_read_word(addr + reno);
	else				// CCW
		return nvm_read_word(addr + reno + 2);
}


/**
 * read eeprom to buffer. return read bytes. (for test and debug.)
 */
uint8_t read_nvm_block(uint8_t start, uint8_t bytes, uint8_t* buffer) {
	int8_t i;
	for(i = 0; i < bytes; i++) {
		if (i >= EEPROM_END)
			break;
		buffer[i] = eeprom_read_byte(EEPROM_START + start + i);
	}
	return i;
}

/**
 * read macro ptrs for debug.
 */
uint8_t read_macro_ptrs(uint8_t bytes, uint8_t * buffer) {
	int8_t i;
	for(i = 0; i < bytes; i++) {
		if (i >= MACRO_KEY_COUNT)
			break;
		buffer[i] = macro_ptrs[i];
	}
	return i;
}

 
void init_nvm_keymap() {
	if (!is_nvm_valid())
		reset_nvm();
	build_macro_index();
}