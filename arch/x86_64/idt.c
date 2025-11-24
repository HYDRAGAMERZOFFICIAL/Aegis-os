#include <kernel/interrupt.h>
#include <kernel/types.h>

#define IDT_ENTRY_COUNT 256
#define IDT_TYPE_INTERRUPT 0x8E
#define IDT_TYPE_TRAP 0x8F

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
} idtr_t;

static idt_entry_t idt[IDT_ENTRY_COUNT] = {0};

extern void exception_handler_0(void);
extern void exception_handler_1(void);
extern void exception_handler_13(void);

void x86_64_init_idt(void)
{
    for (int i = 0; i < IDT_ENTRY_COUNT; i++) {
        idt[i].type_attr = IDT_TYPE_INTERRUPT;
        idt[i].selector = 0x08;
        idt[i].ist = 0;
        idt[i].reserved = 0;
    }
    
    idt[0].offset_low = (u16)(u64)exception_handler_0;
    idt[0].offset_mid = (u16)((u64)exception_handler_0 >> 16);
    idt[0].offset_high = (u32)((u64)exception_handler_0 >> 32);
    
    idt[1].offset_low = (u16)(u64)exception_handler_1;
    idt[1].offset_mid = (u16)((u64)exception_handler_1 >> 16);
    idt[1].offset_high = (u32)((u64)exception_handler_1 >> 32);
    
    idt[13].offset_low = (u16)(u64)exception_handler_13;
    idt[13].offset_mid = (u16)((u64)exception_handler_13 >> 16);
    idt[13].offset_high = (u32)((u64)exception_handler_13 >> 32);
}

void x86_64_load_idt(void)
{
    idtr_t idtr;
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (u64)&idt;
    
    asm volatile("lidt %0" : : "m"(idtr));
    asm volatile("sti");
}
