#pragma once

#include "interrupts.h"

extern uint8_t LAST_SCANCODE;
extern uint8_t INPUT_READY;
extern uint8_t SHIFT_PRESSED;
extern uint8_t CAPS_TOGGLE;

extern const uint8_t SCANCODESLIST[];

uint8_t scancode_to_ascii(uint8_t scancode);
