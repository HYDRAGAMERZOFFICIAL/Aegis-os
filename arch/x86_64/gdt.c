#include <kernel/types.h>

typedef struct {
    u16 limit;
    u64 base;
} gdt_ptr_t;

typedef struct {
    u16 limit_low;
    u16 base_low;
    u8 base_mid;
    u8 access;
    u8 granularity;
    u8 base_high;
} gdt_entry_t;

typedef struct {
    u32 base_low;
    u32 base_high;
    u32 limit;
    u32 reserved;
} tss_t;

static gdt_entry_t gdt[8] = {0};
static tss_t tss = {0};
static gdt_ptr_t gdt_ptr = {0};

void x86_64_set_gdt_entry(u32 index, u64 base, u64 limit, u8 access, u8 granularity)
{
    gdt_entry_t *entry = &gdt[index];
    entry->base_low = (u16)base;
    entry->base_mid = (u8)(base >> 16);
    entry->base_high = (u8)(base >> 24);
    entry->limit_low = (u16)limit;
    entry->granularity = (u8)((limit >> 16) & 0x0F) | granularity;
    entry->access = access;
}

void x86_64_load_gdt(void)
{
    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base = (u64)&gdt;
    
    asm volatile(
        "lgdt %0\n\t"
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%ss\n\t"
        "xor %%ax, %%ax\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "ljmp $0x08, $.Lflush_cs\n\t"
        ".Lflush_cs:\n\t"
        :
        : "m"(gdt_ptr)
        : "ax"
    );
}

void x86_64_setup_tss(u64 rsp0)
{
    tss.base_low = (u32)rsp0;
    tss.base_high = (u32)(rsp0 >> 32);
    tss.limit = sizeof(tss);
    
    x86_64_set_gdt_entry(4, (u64)&tss, sizeof(tss), 0x89, 0x00);
}

void x86_64_load_tss(void)
{
    asm volatile("ltr $0x20");
}
