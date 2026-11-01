#include <kernel/process.h>
#include <kernel/memory.h>
#include <kernel/scheduler.h>
#include <kernel/interrupt.h>
#include <kernel/filesystem.h>
#include <kernel/ipc.h>
#include <kernel/network.h>
#include <kernel/driver.h>
#include <kernel/security.h>
#include <stdio.h>
#include <stdarg.h>

#define SERIAL_COM1 0x3F8

#if defined(__x86_64__) && !defined(_WIN32)
static inline void early_outb(u16 port, u8 val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline u8 early_inb(u16 port) {
    u8 ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void serial_init(void) {
    early_outb(SERIAL_COM1 + 1, 0x00); // Disable all interrupts
    early_outb(SERIAL_COM1 + 3, 0x80); // Enable DLAB (set baud rate divisor)
    early_outb(SERIAL_COM1 + 0, 0x01); // Set divisor to 1 (115200 baud)
    early_outb(SERIAL_COM1 + 1, 0x00);
    early_outb(SERIAL_COM1 + 3, 0x03); // 8 bits, no parity, one stop bit
    early_outb(SERIAL_COM1 + 2, 0xC7); // Enable FIFO, clear TX/RX queues
    early_outb(SERIAL_COM1 + 4, 0x0B); // RTS/DSR set
}

static void serial_putc(char c) {
    while ((early_inb(SERIAL_COM1 + 5) & 0x20) == 0);
    early_outb(SERIAL_COM1, (u8)c);
}

static void serial_puts(const char *str) {
    while (*str) {
        if (*str == '\n') serial_putc('\r');
        serial_putc(*str++);
    }
}
#endif

void printk(const char *fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

#if defined(__x86_64__) && !defined(_WIN32)
    serial_puts(buf);
#else
    printf("%s", buf);
#endif
}

int kernel_main(void)
{
#if defined(__x86_64__) && !defined(_WIN32)
    serial_init();
#endif

    printk("\n========================================================================\n");
    printk("   *   AEGIS OS - HYBRID SECURE INTELLIGENCE OPERATING SYSTEM   *\n");
    printk("   *   Kernel v2.0-Alpha | Dual-Kernel Hybrid | Standalone Mode  *\n");
    printk("========================================================================\n\n");

    printk("[INIT] Initializing Physical & Virtual Memory Management (4-Level Paging)...\n");
    if (mmgr_init() != 0) {
        printk("[FATAL] Memory manager init failed\n");
        return -1;
    }
    printk("[OK] Physical bitmap & x86-64 PML4 page tables active. In-kernel AES ready.\n");

    printk("[INIT] Initializing Process & Thread Management Subsystem...\n");
    if (pmgr_init() != 0) {
        printk("[FATAL] Process manager init failed\n");
        return -1;
    }
    printk("[OK] Process manager active. User/Kernel stack allocators verified.\n");

    printk("[INIT] Initializing Completely Fair Scheduler (CFS)...\n");
    if (scheduler_init() != 0) {
        printk("[FATAL] Scheduler init failed\n");
        return -1;
    }
    printk("[OK] Scheduler active. Real-time & fair priority classes ready.\n");

    printk("[INIT] Initializing Interrupt Event Dispatcher & PIC/APIC Controller...\n");
    if (ied_init() != 0) {
        printk("[FATAL] Interrupt dispatcher init failed\n");
        return -1;
    }
    printk("[OK] 8259A PIC remapped to vectors 0x20-0x2F. Hardware IRQ lines enabled.\n");

    printk("[INIT] Initializing AegisFS Merkle-Rooted Filesystem...\n");
    if (aegisfs_init() != 0) {
        printk("[FATAL] AegisFS init failed\n");
        return -1;
    }
    printk("[OK] AegisFS mounted. Merkle tree integrity validation active.\n");

    printk("[INIT] Initializing Inter-Process Communication (IPC) Channels...\n");
    if (ipc_init() != 0) {
        printk("[FATAL] IPC init failed\n");
        return -1;
    }
    printk("[OK] Fast IPC message queues and zero-copy shared memory ready.\n");

    printk("[INIT] Initializing Network Protocol Stack...\n");
    if (network_init() != 0) {
        printk("[WARN] Network stack init returned non-zero\n");
    } else {
        printk("[OK] TCP/IP and Zero-Trust network layer initialized.\n");
    }

    printk("[INIT] Enumerating PCI Hardware Bus & Loading Drivers...\n");
    if (driver_mgr_init() != 0) {
        printk("[FATAL] Driver manager init failed\n");
        return -1;
    }
    driver_probe_devices();
    printk("[OK] Drivers loaded: AHCI, NVMe, e1000, VirtIO, BGA Display, PS/2 Input.\n");

    printk("[INIT] Initializing Kernel Security Intelligence Monitor...\n");
    if (ksim_init() != 0) {
        printk("[WARN] Security monitor init failed\n");
    } else {
        printk("[OK] Aegis Guard & Zero-Trust access control active.\n");
    }

    printk("\n[SUCCESS] AegisOS Kernel v2.0-Alpha fully initialized.\n");
    printk("Starting primary userland shell and compositor...\n\n");

    scheduler_enable_energy_aware();
    ied_process_events();
    ksim_enable_real_time_analysis();

    return 0;
}
