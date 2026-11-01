/*
 * AegisOS Higher-Half Linker Script for x86-64
 * Targets Higher-Half virtual address 0xffffffff80000000 (-2GB)
 * Physical load address: 0x100000 (1MB)
 */

OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

PHDRS
{
    requests PT_LOAD FLAGS((1 << 1) | (1 << 2)); /* Read-Write */
    text     PT_LOAD FLAGS((1 << 0) | (1 << 2)); /* Read-Execute */
    rodata   PT_LOAD FLAGS((1 << 2));            /* Read-Only */
    data     PT_LOAD FLAGS((1 << 1) | (1 << 2)); /* Read-Write */
}

SECTIONS
{
    /* Kernel loaded at -2 GiB in higher half virtual address space */
    . = 0xffffffff80000000;

    _kernel_start = .;

    /* Limine bootloader request structures */
    .requests ALIGN(4K) : AT(ADDR(.requests) - 0xffffffff80000000 + 0x100000)
    {
        KEEP(*(.requests_start_marker))
        KEEP(*(.requests))
        KEEP(*(.requests_end_marker))
    } :requests

    . = ALIGN(4K);

    /* Executable kernel code */
    .text ALIGN(4K) : AT(ADDR(.text) - 0xffffffff80000000 + 0x100000)
    {
        *(.text .text.*)
    } :text

    . = ALIGN(4K);

    /* Read-only data */
    .rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xffffffff80000000 + 0x100000)
    {
        *(.rodata .rodata.*)
    } :rodata

    . = ALIGN(4K);

    /* Read-write initialized data */
    .data ALIGN(4K) : AT(ADDR(.data) - 0xffffffff80000000 + 0x100000)
    {
        *(.data .data.*)
    } :data

    /* Uninitialized data / BSS */
    .bss ALIGN(4K) : AT(ADDR(.bss) - 0xffffffff80000000 + 0x100000)
    {
        *(COMMON)
        *(.bss .bss.*)
    } :data

    . = ALIGN(4K);
    _kernel_end = .;

    /DISCARD/ :
    {
        *(.eh_frame)
        *(.note .note.*)
    }
}

