/* linker.ld */
OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)
SECTIONS
{
    . = 0x00100000; /* Start address */

    .text : {
        *(.multiboot_header)
        *(.text)
    }

    .rodata : { *(.rodata) }

    .data : { *(.data) }

    .bss : { *(.bss) }
}
