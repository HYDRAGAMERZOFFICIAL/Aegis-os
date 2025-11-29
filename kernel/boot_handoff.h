#ifndef AEGIS_BOOT_HANDOFF_H
#define AEGIS_BOOT_HANDOFF_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define BOOT_HANDOFF_MAGIC 0xAEG15000
#define BOOT_HANDOFF_VERSION 2
#define BOOT_MAX_BOOT_MODULES 64
#define BOOT_MAX_MEMORY_REGIONS 128
#define BOOT_MAX_FRAMEBUFFER_INFO 16

typedef enum {
    BOOT_STATUS_SUCCESS = 0,
    BOOT_STATUS_INVALID_MAGIC = -1,
    BOOT_STATUS_INVALID_VERSION = -2,
    BOOT_STATUS_MEMORY_ERROR = -3,
    BOOT_STATUS_MODULE_ERROR = -4,
    BOOT_STATUS_DEVICE_TREE_ERROR = -5,
    BOOT_STATUS_FRAMEBUFFER_ERROR = -6,
    BOOT_STATUS_ACPI_ERROR = -7,
    BOOT_STATUS_INVALID_STATE = -8
} boot_status_t;

typedef enum {
    BOOT_BOOTLOADER_GRUB = 1,
    BOOT_BOOTLOADER_MULTIBOOT = 2,
    BOOT_BOOTLOADER_UEFI = 3,
    BOOT_BOOTLOADER_UBOOT = 4,
    BOOT_BOOTLOADER_COREBOOT = 5,
    BOOT_BOOTLOADER_CUSTOM = 99
} boot_bootloader_type_t;

typedef enum {
    BOOT_ARCH_X86_64 = 1,
    BOOT_ARCH_ARM32 = 2,
    BOOT_ARCH_ARM64 = 3,
    BOOT_ARCH_RISCV64 = 4
} boot_architecture_t;

typedef struct {
    uint64_t address;
    uint64_t size;
    uint32_t type;
    uint32_t flags;
} boot_memory_region_t;

typedef struct {
    uint64_t address;
    uint64_t size;
    char name[256];
    char cmdline[512];
    uint32_t type;
    uint32_t flags;
} boot_module_t;

typedef struct {
    uint64_t address;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t bpp;
    uint8_t type;
    uint32_t pixel_format;
} boot_framebuffer_t;

typedef struct {
    uint64_t rsdp_address;
    uint32_t rsdp_length;
    uint32_t revision;
    uint64_t xsdt_address;
    bool valid;
} boot_acpi_info_t;

typedef struct {
    uint64_t device_tree_address;
    uint32_t device_tree_size;
    char bootloader_name[256];
    char boot_cmdline[1024];
    bool device_tree_valid;
} boot_device_info_t;

typedef struct {
    uint32_t magic;
    uint32_t version;
    boot_bootloader_type_t bootloader_type;
    boot_architecture_t architecture;
    uint32_t flags;
    
    uint32_t bootloader_id;
    uint32_t api_version;
    
    boot_memory_region_t memory_regions[BOOT_MAX_MEMORY_REGIONS];
    uint32_t memory_region_count;
    
    boot_module_t boot_modules[BOOT_MAX_BOOT_MODULES];
    uint32_t boot_module_count;
    
    boot_framebuffer_t framebuffer;
    uint32_t framebuffer_count;
    
    boot_acpi_info_t acpi_info;
    boot_device_info_t device_info;
    
    uint64_t kernel_entry_point;
    uint64_t kernel_virtual_base;
    uint32_t kernel_flags;
    
    uint64_t initrd_address;
    uint64_t initrd_size;
    
    uint64_t boot_time_ms;
    uint32_t checksum;
} boot_handoff_t;

boot_handoff_t *boot_create_handoff(void);
boot_status_t boot_validate_handoff(const boot_handoff_t *handoff);
boot_status_t boot_validate_multiboot2(const void *mbi, uint32_t magic);
boot_status_t boot_parse_multiboot2(const void *mbi, uint32_t magic, boot_handoff_t *handoff);
boot_status_t boot_validate_arm(const void *arm_info);
boot_status_t boot_parse_arm_boot(const void *arm_info, boot_handoff_t *handoff);

int boot_add_memory_region(boot_handoff_t *handoff, uint64_t addr, uint64_t size, uint32_t type);
int boot_add_module(boot_handoff_t *handoff, uint64_t addr, uint64_t size,
                    const char *name, const char *cmdline);
int boot_set_framebuffer_info(boot_handoff_t *handoff, uint64_t addr, uint32_t width,
                              uint32_t height, uint32_t pitch, uint8_t bpp);
int boot_set_acpi_info(boot_handoff_t *handoff, uint64_t rsdp_addr, uint32_t revision);
int boot_set_device_tree(boot_handoff_t *handoff, uint64_t dtb_addr, uint32_t dtb_size);

bool boot_is_valid_memory_layout(const boot_handoff_t *handoff);
bool boot_validate_modules(const boot_handoff_t *handoff);
bool boot_has_framebuffer(const boot_handoff_t *handoff);
bool boot_has_acpi(const boot_handoff_t *handoff);
bool boot_has_device_tree(const boot_handoff_t *handoff);

void boot_dump_handoff_info(const boot_handoff_t *handoff);
uint32_t boot_calculate_checksum(boot_handoff_t *handoff);
void boot_free_handoff(boot_handoff_t *handoff);

#endif
