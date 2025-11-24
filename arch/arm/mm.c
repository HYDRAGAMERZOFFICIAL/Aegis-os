#include <kernel/types.h>

#define SCTLR_M (1 << 0)
#define SCTLR_C (1 << 2)
#define SCTLR_I (1 << 12)
#define TCR_TG0_4K (0 << 14)

typedef struct {
    u64 entries[512];
} ttbr_t;

static ttbr_t kernel_ttbr = {0};

void arm_enable_mmu(void)
{
    u64 sctlr;
    
    asm volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    sctlr |= (SCTLR_M | SCTLR_C | SCTLR_I);
    asm volatile("msr sctlr_el1, %0" : : "r"(sctlr));
    
    asm volatile("isb");
}

void arm_set_ttbr0(void *ttbr)
{
    asm volatile("msr ttbr0_el1, %0" : : "r"(ttbr));
    asm volatile("isb");
}

void arm_invalidate_tlb(void)
{
    asm volatile("tlbi vmalle1; isb");
}

void arm_enable_btac(void)
{
    u64 sctlr;
    asm volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    sctlr |= (1UL << 11);
    asm volatile("msr sctlr_el1, %0" : : "r"(sctlr));
}

void arm_enable_pxn(void)
{
    u64 sctlr;
    asm volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    sctlr |= (1UL << 27);
    asm volatile("msr sctlr_el1, %0" : : "r"(sctlr));
}

void arm_setup_vbar(u64 vbar_base)
{
    asm volatile("msr vbar_el1, %0" : : "r"(vbar_base));
    asm volatile("isb");
}
