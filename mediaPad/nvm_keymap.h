// nvm_keymap.h
#ifndef NVM_KEYMAP_H
#define NVM_KEYMAP_H

void init_nvm_keymap();
void reset_nvm();
uint16_t read_nvm_code(uint8_t sw, uint8_t layer);
void write_nvm_code(uint8_t sw, uint8_t layer, uint16_t code);
uint16_t nvm_read_word(uint8_t addr);
void nvm_write_word(uint8_t addr, uint16_t data);

int8_t read_keys(int8_t start_sw, int8_t count, int8_t layer, uint16_t* buffer);
int8_t write_keys(int8_t start_sw, int8_t count, int8_t layer, uint16_t* buffer);
int8_t read_macros(int8_t start_index, int8_t bytes, char* buffer);
uint8_t write_macros(int8_t start_index, int8_t bytes, uint8_t* buffer);

int8_t write_re_keys(int8_t reno, int8_t layer, int8_t count, uint16_t* buffer);
int8_t read_re_keys(int8_t reno, int8_t layer, int8_t count, uint16_t* buffer);

void build_macro_index();   
// for test and debug.
uint8_t read_nvm_block(uint8_t start, uint8_t bytes, uint8_t* buffer);
uint8_t read_macro_ptrs(uint8_t bytes, uint8_t * buffer);

uint16_t get_re_keycode(int8_t reno, int8_t layer, uint8_t direction);


extern __xdata uint8_t macro_ptrs[];


//
#endif