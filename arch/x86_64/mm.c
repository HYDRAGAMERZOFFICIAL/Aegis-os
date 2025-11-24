#include <kernel/types.h>

#define CR0_PG (1UL << 31)
#define CR4_PAE (1UL << 5)
#define CR4_PSE (1UL << 4)

typedef struct {
    u64 entries[512];
} pml4_t;

static pml4_t kernel_pml4 = {0};

void x86_64_enable_paging(void)
{
    u64 cr0, cr4;
    
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (CR4_PAE | CR4_PSE);
    asm volatile("mov %0, %%cr4" : : "r"(cr4));
    
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= CR0_PG;
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
}

void x86_64_set_pml4(void *pml4_addr)
{
    asm volatile("mov %0, %%cr3" : : "r"(pml4_addr));
}

void x86_64_flush_tlb(void)
{
    u64 cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    asm volatile("mov %0, %%cr3" : : "r"(cr3));
}

void x86_64_enable_nxe(void)
{
    u64 msr = 0xC0000080;
    u64 val;
    
    asm volatile("rdmsr" : "=A"(val) : "c"(msr));
    val |= (1UL << 11);
    asm volatile("wrmsr" : : "A"(val), "c"(msr));
}

void x86_64_enable_smap(void)
{
    u64 cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1UL << 21);
    asm volatile("mov %0, %%cr4" : : "r"(cr4));
}

void x86_64_enable_smep(void)
{
    u64 cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1UL << 20);
    asm volatile("mov %0, %%cr4" : : "r"(cr4));
}
