[bits 32]

extern keyboard_handler

global _keyboard_int_handler
global _default_int_handler

align 4

_default_int_handler:
    iret

_keyboard_int_handler:
    pushad
    cld    ; C code following the sysV ABI requires DF to be clear on function entry
    call keyboard_handler
    popad
    iretd
