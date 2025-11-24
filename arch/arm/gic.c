#include <kernel/types.h>

#define GICD_BASE 0xFF841000
#define GICC_BASE 0xFF842000

typedef struct {
    u32 ctlr;
    u32 typer;
    u32 iidr;
    u8 reserved1[116];
    u32 igroupr[32];
    u32 isenabler[32];
    u32 icenabler[32];
    u32 ispendr[32];
    u32 icpendr[32];
} gicd_t;

typedef struct {
    u32 ctlr;
    u32 pmr;
    u32 bpr;
    u32 iar;
    u32 eoir;
    u32 rpr;
    u32 hppir;
} gicc_t;

static volatile gicd_t *gicd = (volatile gicd_t *)GICD_BASE;
static volatile gicc_t *gicc = (volatile gicc_t *)GICC_BASE;

void arm_gic_init(void)
{
    gicd->ctlr = 1;
    
    for (int i = 0; i < 32; i++) {
        gicd->icenabler[i] = 0xFFFFFFFF;
        gicd->icpendr[i] = 0xFFFFFFFF;
    }
    
    gicc->ctlr = 1;
    gicc->pmr = 0xFF;
    gicc->bpr = 0;
}

void arm_gic_enable_irq(u32 irq)
{
    u32 byte_offset = irq / 32;
    u32 bit_offset = irq % 32;
    gicd->isenabler[byte_offset] = (1U << bit_offset);
}

void arm_gic_disable_irq(u32 irq)
{
    u32 byte_offset = irq / 32;
    u32 bit_offset = irq % 32;
    gicd->icenabler[byte_offset] = (1U << bit_offset);
}

u32 arm_gic_get_irq(void)
{
    return gicc->iar & 0x3FF;
}

void arm_gic_eoi(u32 irq)
{
    gicc->eoir = irq;
}

void arm_gic_set_target(u32 irq, u8 cpu_mask)
{
    u32 byte_offset = irq / 4;
    u32 bit_offset = (irq % 4) * 8;
    
    u32 *reg = (u32 *)((u8 *)&gicd->igroupr[32] + byte_offset);
    *reg = (*reg & ~(0xFF << bit_offset)) | (cpu_mask << bit_offset);
}
