; kernel_entry.asm
section .text
global _start
_start:
    ; Call the main function in C
    extern kmain
    call kmain

    ; Halt the CPU
    cli
    hlt
