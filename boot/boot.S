.section ".text.boot"
.global _start

_start:
    // Check processor ID is 0 (primary core)
    mrc p15, 0, r1, c0, c0, 5
    and r1, r1, #3
    cmp r1, #0
    bne halt

    // Set up stack pointer
    mov sp, #0x8000

    // Clear BSS section
    ldr r4, =__bss_start
    ldr r9, =__bss_end
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
2:
    cmp r4, r9
    bhs 3f
1:
    stmia r4!, {r5-r8}
    b 2b

3:
    // Set up interrupt vector table
    ldr r0, =_vector_table
    mcr p15, 0, r0, c12, c0, 0  // Set VBAR

    // Enable IRQ
    mrs r0, cpsr
    bic r0, r0, #0x80
    msr cpsr_c, r0

    // Jump to kernel_main
    bl kernel_main

halt:
    wfe
    b halt

.align 5
_vector_table:
    ldr pc, reset_handler
    ldr pc, undefined_handler
    ldr pc, svc_handler
    ldr pc, prefetch_abort_handler
    ldr pc, data_abort_handler
    ldr pc, hyp_handler
    ldr pc, irq_handler
    ldr pc, fiq_handler

reset_handler:          .word _start
undefined_handler:      .word halt
svc_handler:            .word halt
prefetch_abort_handler: .word halt
data_abort_handler:     .word halt
hyp_handler:            .word halt
irq_handler:            .word irq_handler
fiq_handler:            .word halt
