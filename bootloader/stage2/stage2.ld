OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)
/* ENTRY(stage2_entry) */
ENTRY(main)
SECTIONS {
    . = 0xBC00;
    .text : { *(.text) }
    .data : { *(.data) }
    .rodata : { *(.rodata) }
    .bss : { *(.bss) }
    /DISCARD/ : { *(.note*) }
}
