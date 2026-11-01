#include <kernel/types.h>

#pragma pack(push, 1)
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
#pragma pack(pop)

static idt_entry_t idt[256] = {0};

void x86_64_set_idt_entry(u32 vector, u64 handler, u8 type)
{
    if (vector >= 256) return;
    idt_entry_t *entry = &idt[vector];
    entry->offset_low = (u16)(handler & 0xFFFF);
    entry->offset_mid = (u16)((handler >> 16) & 0xFFFF);
    entry->offset_high = (u32)(handler >> 32);
    entry->selector = 0x08; // Kernel code segment
    entry->type_attr = type;
    entry->ist = 0;
    entry->reserved = 0;
}

void x86_64_load_idt(void)
{
    idt_ptr_t idt_ptr;
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (u64)&idt[0];
    
#if defined(__x86_64__) && !defined(_WIN32)
    asm volatile("lidt %0" : : "m"(idt_ptr));
#endif
}

void x86_64_enable_irq(void)
{
#if defined(__x86_64__) && !defined(_WIN32)
    asm volatile("sti");
#endif
}

void x86_64_disable_irq(void)
{
#if defined(__x86_64__) && !defined(_WIN32)
    asm volatile("cli");
#endif
}

u64 x86_64_get_rflags(void)
{
#if defined(__x86_64__) && !defined(_WIN32)
    u64 rflags;
    asm volatile("pushfq; popq %0" : "=r"(rflags));
    return rflags;
#else
    return 0x202; // IF enabled simulation flag
#endif
}

void x86_64_set_rflags(u64 rflags)
{
#if defined(__x86_64__) && !defined(_WIN32)
    asm volatile("pushq %0; popfq" : : "r"(rflags));
#endif
}

void x86_64_send_eoi(void)
{
#if defined(__x86_64__) && !defined(_WIN32)
    asm volatile("mov $0x20, %%al; outb %%al, $0x20" : : : "al");
#endif
}
