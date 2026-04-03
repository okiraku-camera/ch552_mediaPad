#include "Arduino.h"
#pragma once


void set_color_data(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void send_rgb_to_all(uint8_t r, uint8_t g, uint8_t b );
void send_data(uint8_t* data );
void sled_off() ;
void serial_led_init();
void sled_max();
void sled_dim();
uint8_t get_sled_value();
void change_led_value(int8_t delta);
void hue_to_led(int16_t hue);

