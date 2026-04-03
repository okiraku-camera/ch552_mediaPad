/**
 * r_encoder.c get encoder counts and dir of rotation.
 * Copyright (c) 2024 Takeshi Higasa, okiraku-camera.tokyo
 * 
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 * 
 */

#include "r_encoder.h"
#define ENC_A	33
#define ENC_B	34
static int16_t enc_counter = 0;
static int8_t prevEncoderState = 0;

void encoder_init() {
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
	enc_counter = 0;
	prevEncoderState = 0;
}

int16_t get_encoder_count() {
	int8_t encoderState = digitalRead(ENC_A) | (digitalRead(ENC_B) << 1);
	int8_t stateChange = encoderState - prevEncoderState;
	if (encoderState == 2) {
		if (stateChange == 2)
			enc_counter--;
		else if (stateChange == -1)
			enc_counter++;
	}
	prevEncoderState = encoderState;
	return enc_counter;
}
