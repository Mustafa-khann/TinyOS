SECTIONS
{
    /* QEMU loads raw -kernel binaries at 0x10000 on the raspi machines,
     * so link there to keep linked == loaded addresses. On real hardware
     * set `kernel_address=0x10000` in config.txt to match. */
    . = 0x10000;
    __start = .;

    .text : {
        KEEP(*(.text.boot))
        *(.text)
    }

    .rodata : {
        __rodata_start = .;
        *(.rodata)
        *(.rodata.*)
        KEEP(*(.font))  /* Explicitly keep font data */
        __rodata_end = .;
    }

    .data : {
        __data_start = .;
        *(.data)
        __data_end = .;
    }

    . = ALIGN(0x1000);
    __bss_start = .;
    .bss : {
        *(.bss)
    }
    __bss_end = .;

    __end = .;
}
