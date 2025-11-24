#include <kernel/process.h>
#include <kernel/memory.h>
#include <kernel/scheduler.h>
#include <kernel/interrupt.h>
#include <kernel/filesystem.h>
#include <kernel/ipc.h>
#include <kernel/network.h>
#include <kernel/driver.h>
#include <kernel/security.h>

void printk(const char *fmt, ...);

int kernel_main(void)
{
    printk("Aegis OS Kernel Initializing...\n");
    
    if (mmgr_init() != 0) {
        printk("ERROR: Memory manager init failed\n");
        return -1;
    }
    printk("Memory manager initialized\n");
    
    if (pmgr_init() != 0) {
        printk("ERROR: Process manager init failed\n");
        return -1;
    }
    printk("Process manager initialized\n");
    
    if (scheduler_init() != 0) {
        printk("ERROR: Scheduler init failed\n");
        return -1;
    }
    printk("Scheduler initialized\n");
    
    if (ied_init() != 0) {
        printk("ERROR: Interrupt dispatcher init failed\n");
        return -1;
    }
    printk("Interrupt dispatcher initialized\n");
    
    if (aegisfs_init() != 0) {
        printk("ERROR: AegisFS init failed\n");
        return -1;
    }
    printk("AegisFS initialized\n");
    
    if (ipc_init() != 0) {
        printk("ERROR: IPC init failed\n");
        return -1;
    }
    printk("IPC system initialized\n");
    
    if (network_init() != 0) {
        printk("ERROR: Network stack init failed\n");
        return -1;
    }
    printk("Network stack initialized\n");
    
    if (driver_mgr_init() != 0) {
        printk("ERROR: Driver manager init failed\n");
        return -1;
    }
    printk("Driver manager initialized\n");
    
    if (ksim_init() != 0) {
        printk("ERROR: Security monitor init failed\n");
        return -1;
    }
    printk("Security monitor initialized\n");
    
    printk("Aegis OS kernel initialized successfully\n");
    
    scheduler_enable_energy_aware();
    ied_process_events();
    ksim_enable_real_time_analysis();
    
    return 0;
}

void printk(const char *fmt, ...)
{
}
