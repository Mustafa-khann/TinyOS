SECTIONS
{
    . = 0x8000;
    .text : { *(.text.boot) *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    . = ALIGN(0x1000);
    __bss_start = .;
    .bss : { *(.bss) }
    __bss_end = .;
}
