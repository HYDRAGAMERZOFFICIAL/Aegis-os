#include <kernel/types.h>

typedef struct {
    u64 x0, x1, x2, x3, x4, x5, x6, x7;
    u64 x8, x9, x10, x11, x12, x13, x14, x15;
    u64 x16, x17, x18, x19, x20, x21, x22, x23;
    u64 x24, x25, x26, x27, x28, x29, x30;
} exception_frame_t;

extern void el1_sync_handler(exception_frame_t *frame);

void arm_enable_irq(void)
{
    asm volatile("msr daifclr, #2");
}

void arm_disable_irq(void)
{
    asm volatile("msr daifset, #2");
}

u64 arm_get_daif(void)
{
    u64 daif;
    asm volatile("mrs %0, daif" : "=r"(daif));
    return daif;
}

void arm_set_daif(u64 daif)
{
    asm volatile("msr daif, %0" : : "r"(daif));
}

void arm_setup_exception_vectors(u64 vector_base)
{
    asm volatile("msr vbar_el1, %0" : : "r"(vector_base));
    asm volatile("isb");
}

void arm_send_sgi(u32 irq, u32 cpu_mask)
{
    u64 sgi_val = ((u64)irq << 24) | (1UL << 15) | cpu_mask;
    asm volatile("msr icc_sgi1r_el1, %0" : : "r"(sgi_val));
}
