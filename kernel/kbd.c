#include "kbd.h"

const uint8_t SCANCODESLIST[] = {
    0x00,
    0x1B, // Escape      0x01
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    0x08, // Backspace   0x0E
    0x09, // Tab         0x0F
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    0x0A, // Enter       0x1C
    0x00, // Ctrl        0x1D
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
    '\`',
    0x00, // Shift       0x2A
    '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0x00,
    0x00,
    0x00, // Alt         0x38
    ' ',  // Space       0x39
    0x00,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F1-F10 0x3B-0x44
    0x00,
    0x00,
    0x00, // Home        0x47
    0x00, // Arrow up    0x48
    0x00, // Page up     0x49
    0x00,
    0x00, // Arrow left  0x4B
    0x00,
    0x00, // Arrow right 0x4D
    0x00,
    0x00, // End         0x4F
    0x00, // Arrow down  0x50
    0x00, // Page down   0x51
    0x00, // Insert      0x52
    0x00, // Delete      0x53
    0, 0, 0,
    0, 0 // F11-F12      0x57-0x58
};

const uint8_t SCANCODESLIST_SHIFT[] = {
    0x00,
    0x1B, // Escape      0x01
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
    0x08, // Backspace   0x0E
    0x09, // Tab         0x0F
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
    0x0A, // Enter       0x1C
    0x00, // Ctrl        0x1D
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"',
    '\`',
    0x00, // Shift       0x2A
    '\\',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0x00,
    0x00,
    0x00, // Alt         0x38
    ' ',  // Space       0x39
    0x00, // Tab         0x3A
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F1-F10 0x3B-0x44
    0x00,
    0x00,
    0x00, // Home        0x47
    0x00, // Arrow up    0x48
    0x00, // Page up     0x49
    0x00,
    0x00, // Arrow left  0x4B
    0x00,
    0x00, // Arrow right 0x4D
    0x00,
    0x00, // End         0x4F
    0x00, // Arrow down  0x50
    0x00, // Page down   0x51
    0x00, // Insert      0x52
    0x00, // Delete      0x53
    0, 0, 0,
    0, 0 // F11-F12      0x57-0x58
};

uint8_t LAST_SCANCODE = 0xFF;
uint8_t INPUT_READY = 0;
uint8_t SHIFT_PRESSED = 0;
uint8_t CAPS_TOGGLE = 0;

uint8_t scancode_to_ascii(uint8_t scancode)
{
    uint8_t ascii;
    if ( SHIFT_PRESSED )
        ascii = SCANCODESLIST_SHIFT[scancode];
    else
        ascii = SCANCODESLIST[scancode];

    // TODO: NOT WORKING
    /* if ( CAPS_TOGGLE && ascii >= 0x61 && ascii <= 0x7A ) */
    /*     ascii -= 32; */

    /* if ( CAPS_TOGGLE && ascii >= 0x41 && ascii <= 0x5A ) */
    /*     ascii += 32; */

    return ascii;
}
