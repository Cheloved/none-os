OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)
ENTRY(main)
SECTIONS {
    . = 0x8600;
    .text : { *(.text) }
    .data : { *(.data) }
    .rodata : { *(.rodata) }
    .bss : { *(.bss) }
    /DISCARD/ : { *(.note*) }
}
