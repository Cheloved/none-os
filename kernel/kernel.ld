OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)
ENTRY(main)
SECTIONS {
    . = 0x10000;
    .text : { *(.text) }
    .data : { *(.data) }
    .rodata : { *(.rodata) }
    .bss : { *(.bss) }
    /DISCARD/ : { *(.note*) }
}
