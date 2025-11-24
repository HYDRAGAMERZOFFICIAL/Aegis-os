#include <kernel/types.h>

typedef struct {
    u16 offset_low;
    u16 selector;
    u8 ist;
    u8 type_attr;
    u16 offset_mid;
    u32 offset_high;
    u32 reserved;
} idt_entry_t;

typedef struct {
    u16 limit;
    u64 base;
} idt_ptr_t;

static idt_entry_t idt[256] = {0};

void x86_64_set_idt_entry(u32 vector, u64 handler, u8 type)
{
    idt_entry_t *entry = &idt[vector];
    entry->offset_low = (u16)handler;
    entry->offset_mid = (u16)(handler >> 16);
    entry->offset_high = (u32)(handler >> 32);
    entry->selector = 0x08;
    entry->type_attr = type;
    entry->ist = 0;
    entry->reserved = 0;
}

void x86_64_load_idt(void)
{
    idt_ptr_t idt_ptr;
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (u64)&idt;
    
    asm volatile("lidt %0" : : "m"(idt_ptr));
}

void x86_64_enable_irq(void)
{
    asm volatile("sti");
}

void x86_64_disable_irq(void)
{
    asm volatile("cli");
}

u64 x86_64_get_rflags(void)
{
    u64 rflags;
    asm volatile("pushfq; popq %0" : "=r"(rflags));
    return rflags;
}

void x86_64_set_rflags(u64 rflags)
{
    asm volatile("pushq %0; popfq" : : "r"(rflags));
}

void x86_64_send_eoi(void)
{
    asm volatile("mov $0x20, %%al; outb %%al, $0x20" : : : "al");
}
