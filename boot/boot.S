.section ".text.boot"

.global _start

_start:
    // Disable all cores except CPU 0
    mrc p15, 0, r0, c0, c0, 5
    and r0, r0, #3
    cmp r0, #0
    bne halt

    // Set up stack pointer
    ldr sp, =0x8000

    // Clear BSS
    ldr r4, =__bss_start
    ldr r9, =__bss_end
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
    b 2f

1:
    stmia r4!, {r5-r8}

2:
    cmp r4, r9
    blo 1b

    // Jump to C code
    bl kernel_main

halt:
    wfe
    b halt
