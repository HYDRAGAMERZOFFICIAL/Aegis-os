#include <kernel/types.h>
#include <kernel/process.h>
#include <kernel/memory.h>
#include <kernel/scheduler.h>
#include <kernel/interrupt.h>
#include <kernel/filesystem.h>
#include <kernel/network.h>
#include <kernel/driver.h>
#include <kernel/security.h>
#include <drivers/driver_manager.h>
#include <security/microkernel.h>
#include <security/crypto_engine.h>
#include <security/aegis_guard.h>
#include <string.h>

static int system_initialized = 0;

int system_init(void) {
    if (system_initialized) {
        return 0;
    }

    printk("Aegis OS: System Initialization Starting...\n");

    printk("[1/12] Initializing Memory Manager...\n");
    if (mmgr_init() != 0) {
        printk("ERROR: Memory Manager initialization failed\n");
        return -1;
    }

    printk("[2/12] Initializing Scheduler...\n");
    if (scheduler_init() != 0) {
        printk("ERROR: Scheduler initialization failed\n");
        return -2;
    }

    printk("[3/12] Initializing Interrupt/Event Dispatcher...\n");
    if (ied_init() != 0) {
        printk("ERROR: IED initialization failed\n");
        return -3;
    }

    printk("[4/12] Initializing Process Manager...\n");
    if (pmgr_init() != 0) {
        printk("ERROR: Process Manager initialization failed\n");
        return -4;
    }

    printk("[5/12] Initializing Filesystem...\n");
    if (aegisfs_init() != 0) {
        printk("ERROR: Filesystem initialization failed\n");
        return -5;
    }

    printk("[6/12] Initializing Network Stack...\n");
    if (network_init() != 0) {
        printk("ERROR: Network initialization failed\n");
        return -6;
    }

    printk("[7/12] Initializing IPC Manager...\n");
    if (ipc_init() != 0) {
        printk("ERROR: IPC initialization failed\n");
        return -7;
    }

    printk("[8/12] Initializing Security Module...\n");
    if (ksim_init() != 0) {
        printk("ERROR: Security initialization failed\n");
        return -8;
    }

    printk("[9/12] Initializing Cryptography Engine...\n");
    if (crypto_init() != 0) {
        printk("ERROR: Cryptography initialization failed\n");
        return -9;
    }

    printk("[10/12] Initializing Microkernel Architecture...\n");
    if (mkern_init() != 0) {
        printk("ERROR: Microkernel initialization failed\n");
        return -10;
    }

    printk("[11/12] Initializing AegisGuard IDS...\n");
    if (guard_init() != 0) {
        printk("ERROR: AegisGuard initialization failed\n");
        return -11;
    }

    printk("[12/12] Initializing Driver Manager...\n");
    if (driver_manager_init() != 0) {
        printk("ERROR: Driver Manager initialization failed\n");
        return -12;
    }

    printk("\nAegis OS: Core System Initialization Complete!\n");
    system_initialized = 1;

    return 0;
}

int system_shutdown(void) {
    if (!system_initialized) {
        return -1;
    }

    printk("\nAegis OS: System Shutdown Starting...\n");

    printk("[1/10] Shutting down Driver Manager...\n");
    driver_manager_shutdown();

    printk("[2/10] Shutting down AegisGuard...\n");
    (void)0;

    printk("[3/10] Shutting down Microkernel...\n");
    (void)0;

    printk("[4/10] Shutting down Security Module...\n");
    (void)0;

    printk("[5/10] Shutting down IPC Manager...\n");
    (void)0;

    printk("[6/10] Shutting down Network Stack...\n");
    (void)0;

    printk("[7/10] Shutting down Filesystem...\n");
    (void)0;

    printk("[8/10] Shutting down Process Manager...\n");
    (void)0;

    printk("[9/10] Shutting down Scheduler...\n");
    (void)0;

    printk("[10/10] Shutting down Memory Manager...\n");
    (void)0;

    printk("Aegis OS: System Shutdown Complete!\n");
    system_initialized = 0;

    return 0;
}

int system_is_initialized(void) {
    return system_initialized;
}

void printk(const char* fmt, ...) {
    (void)fmt;
}
