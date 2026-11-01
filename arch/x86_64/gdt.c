#include <kernel/types.h>
#include <string.h>

/*
 * x86-64 Global Descriptor Table (GDT) and Task State Segment (TSS)
 * Freestanding implementation with 16-byte TSS descriptor for Long Mode.
 */

#pragma pack(push, 1)
typedef struct {
    u16 limit;
    u64 base;
} gdt_ptr_t;

typedef struct {
    u16 limit_low;
    u16 base_low;
    u8  base_mid;
    u8  access;
    u8  granularity;
    u8  base_high;
} gdt_entry_t;

typedef struct {
    gdt_entry_t low;
    u32 base_upper32;
    u32 reserved;
} tss_descriptor_t;

typedef struct {
    u32 reserved0;
    u64 rsp0;
    u64 rsp1;
    u64 rsp2;
    u64 reserved1;
    u64 ist[7];
    u64 reserved2;
    u16 reserved3;
    u16 iopb_offset;
} tss64_t;
#pragma pack(pop)

/*
 * GDT Layout:
 * 0x00: Null Descriptor
 * 0x08: Kernel Code 64-bit (Ring 0, Exec/Read)
 * 0x10: Kernel Data 64-bit (Ring 0, Read/Write)
 * 0x18: User Data 64-bit   (Ring 3, Read/Write)
 * 0x20: User Code 64-bit   (Ring 3, Exec/Read)
 * 0x28: TSS Descriptor     (16 bytes: slots 5 & 6)
 */
static gdt_entry_t gdt[8] = {0};
static tss64_t tss = {0};
static gdt_ptr_t gdt_ptr = {0};
static bool gdt_initialized = false;

void x86_64_set_gdt_entry(u32 index, u64 base, u64 limit, u8 access, u8 granularity)
{
    if (index >= 8) return;
    gdt_entry_t *entry = &gdt[index];
    entry->limit_low = (u16)(limit & 0xFFFF);
    entry->base_low = (u16)(base & 0xFFFF);
    entry->base_mid = (u8)((base >> 16) & 0xFF);
    entry->access = access;
    entry->granularity = (u8)((limit >> 16) & 0x0F) | granularity;
    entry->base_high = (u8)((base >> 24) & 0xFF);
}

void x86_64_setup_tss(u64 rsp0)
{
    memset(&tss, 0, sizeof(tss64_t));
    tss.rsp0 = rsp0;
    tss.iopb_offset = sizeof(tss64_t);

    u64 base = (u64)(uintptr_t)&tss;
    u64 limit = sizeof(tss64_t) - 1;

    // TSS descriptor in x86-64 Long Mode is 16 bytes spanning entries 5 and 6
    x86_64_set_gdt_entry(5, base, limit, 0x89, 0x00); // Type 0x89: Present, 64-bit TSS available

    // Second half of 16-byte TSS descriptor (Slot 6)
    tss_descriptor_t *tss_desc = (tss_descriptor_t *)&gdt[5];
    tss_desc->base_upper32 = (u32)(base >> 32);
    tss_desc->reserved = 0;
}

void x86_64_set_tss_rsp0(u64 rsp0)
{
    tss.rsp0 = rsp0;
}

u64 x86_64_get_tss_rsp0(void)
{
    return tss.rsp0;
}

void x86_64_init_gdt(void)
{
    // Slot 0: Null Descriptor
    x86_64_set_gdt_entry(0, 0, 0, 0, 0);

    // Slot 1 (0x08): Kernel Code 64-bit (Ring 0, Exec/Read, Long Mode bit set in flags)
    x86_64_set_gdt_entry(1, 0, 0xFFFFF, 0x9A, 0x20);

    // Slot 2 (0x10): Kernel Data 64-bit (Ring 0, Read/Write)
    x86_64_set_gdt_entry(2, 0, 0xFFFFF, 0x92, 0x00);

    // Slot 3 (0x18): User Data 64-bit (Ring 3, Read/Write)
    x86_64_set_gdt_entry(3, 0, 0xFFFFF, 0xF2, 0x00);

    // Slot 4 (0x20): User Code 64-bit (Ring 3, Exec/Read, Long Mode bit set)
    x86_64_set_gdt_entry(4, 0, 0xFFFFF, 0xFA, 0x20);

    // Slots 5 & 6 (0x28): TSS Descriptor
    x86_64_setup_tss(0);

    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base = (u64)(uintptr_t)&gdt[0];
    gdt_initialized = true;
}

void x86_64_load_gdt(void)
{
    if (!gdt_initialized) {
        x86_64_init_gdt();
    }

#if defined(__x86_64__) && !defined(_WIN32)
    asm volatile(
        "lgdt %0\n\t"
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%ss\n\t"
        "xor %%ax, %%ax\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "pushq $0x08\n\t"
        "leaq 1f(%%rip), %%rax\n\t"
        "pushq %%rax\n\t"
        "lretq\n\t"
        "1:\n\t"
        :
        : "m"(gdt_ptr)
        : "rax", "memory"
    );
#else
    // Host / simulation mode: record loaded state safely
#endif
}

void x86_64_load_tss(void)
{
#if defined(__x86_64__) && !defined(_WIN32)
    asm volatile("ltr %%ax" : : "a"((u16)0x28));
#else
    // Host / simulation mode: record loaded TSS selector 0x28
#endif
}
