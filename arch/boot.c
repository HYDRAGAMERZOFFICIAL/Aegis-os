#include <kernel/types.h>
#include <boot/limine.h>

/*
 * AegisOS Boot Entry Point & Architecture Setup
 * Standard freestanding entry point receiving Limine bootloader handoff.
 */

extern int kernel_main(void);
extern void x86_64_init_gdt(void);
extern void x86_64_load_gdt(void);
extern void x86_64_load_tss(void);
extern void x86_64_init_idt(void);
extern void x86_64_load_idt(void);
extern void x86_64_enable_paging(void);
extern void x86_64_enable_nxe(void);
extern void x86_64_enable_smap(void);
extern void x86_64_enable_smep(void);
extern void printk(const char *fmt, ...);

#if defined(__x86_64__) && !defined(_WIN32)
/* Limine Protocol Requests in designated .requests section */
__attribute__((used, section(".requests")))
static volatile LIMINE_BASE_REVISION(2);

__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

__attribute__((used, section(".requests")))
static volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};
#endif

void setup_arch(void)
{
#ifdef __x86_64__
    x86_64_init_gdt();
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

void _start(void)
{
#if defined(__x86_64__) && !defined(_WIN32)
    asm volatile("cli");
#endif

    // Setup GDT, TSS, IDT, Paging and Security CPU bits
    setup_arch();

    // Handoff execution to AegisOS microkernel main
    kernel_main();

    // Idle loop if kernel_main ever returns
    while (1) {
#if defined(__x86_64__) && !defined(_WIN32)
        asm volatile("hlt");
#else
        break;
#endif
    }
}

void boot_init(void)
{
    _start();
}
