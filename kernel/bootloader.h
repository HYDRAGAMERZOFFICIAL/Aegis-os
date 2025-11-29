#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    BOOT_REASON_POWER_ON = 0,
    BOOT_REASON_RESTART = 1,
    BOOT_REASON_WATCHDOG = 2,
    BOOT_REASON_RECOVERY = 3,
} BootReason;

typedef enum {
    BOOT_MODE_NORMAL = 0,
    BOOT_MODE_SAFE = 1,
    BOOT_MODE_RECOVERY = 2,
    BOOT_MODE_FIRMWARE = 3,
} BootMode;

typedef struct {
    uint32_t signature;
    uint32_t version;
    uint32_t checksum;
    uint32_t size;
    uint64_t kernel_entry;
    uint64_t kernel_size;
    uint64_t initrd_size;
} BootHeader;

typedef struct {
    uint64_t base;
    uint64_t size;
    uint32_t type;
    uint32_t flags;
} MemoryRegion;

typedef struct {
    BootReason reason;
    BootMode mode;
    BootHeader header;
    MemoryRegion memory_regions[32];
    uint32_t region_count;
    uint64_t timestamp;
    char cmdline[512];
} BootInfo;

int bootloader_init(void);
int bootloader_load_kernel(const char* kernel_path);
int bootloader_load_initrd(const char* initrd_path);
int bootloader_verify_signature(const void* data, size_t size);
int bootloader_setup_mmu(void);
int bootloader_setup_gdt(void);
int bootloader_jump_to_kernel(uint64_t entry_point);
int bootloader_setup_stack(uint64_t stack_base, uint64_t stack_size);
int bootloader_enable_paging(void);
int bootloader_get_boot_info(BootInfo* info);
int bootloader_set_boot_mode(BootMode mode);
BootMode bootloader_get_boot_mode(void);
int bootloader_check_integrity(void);

#endif
