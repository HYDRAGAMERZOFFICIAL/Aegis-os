#include <kernel/types.h>

void kernel_main(void);

void boot_init(void)
{
    asm volatile("cli");
    
    kernel_main();
    
    while (1) {
        asm volatile("hlt");
    }
}

void setup_arch(void)
{
#ifdef __x86_64__
    extern void x86_64_enable_paging(void);
    extern void x86_64_enable_nxe(void);
    extern void x86_64_enable_smap(void);
    extern void x86_64_enable_smep(void);
    extern void x86_64_load_gdt(void);
    extern void x86_64_load_tss(void);
    extern void x86_64_init_idt(void);
    extern void x86_64_load_idt(void);
    
    x86_64_load_gdt();
    x86_64_load_tss();
    x86_64_enable_paging();
    x86_64_enable_nxe();
    x86_64_enable_smap();
    x86_64_enable_smep();
    x86_64_init_idt();
    x86_64_load_idt();
#endif
}
