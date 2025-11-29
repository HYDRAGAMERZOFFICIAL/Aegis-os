#ifndef AEGIS_KERNEL_BOOT_PARAMS_H
#define AEGIS_KERNEL_BOOT_PARAMS_H

#include <stdint.h>

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2badb002

#define MULTIBOOT_INFO_MEMORY       0x00000001
#define MULTIBOOT_INFO_BOOTLOADER_NAME 0x00000200
#define MULTIBOOT_INFO_MEM_MAP      0x00000040
#define MULTIBOOT_INFO_MODULES      0x00000008
#define MULTIBOOT_INFO_CMDLINE      0x00000004

#define MULTIBOOT_MEMORY_AVAILABLE  1
#define MULTIBOOT_MEMORY_RESERVED   2

typedef struct {
    uint32_t mod_start;
    uint32_t mod_end;
    uint32_t cmdline;
    uint32_t pad;
} multiboot_module_t;

typedef struct {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} multiboot_memory_map_t;

typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
} multiboot_info_t;

void boot_params_init(void *multiboot_info_ptr);

uint32_t boot_get_mem_lower(void);

uint32_t boot_get_mem_upper(void);

uint64_t boot_get_total_memory(void);

const char *boot_get_bootloader_name(void);

const char *boot_get_cmdline(void);

uint32_t boot_get_mmap_count(void);

const multiboot_memory_map_t *boot_get_mmap(uint32_t index);

uint32_t boot_get_modules_count(void);

const multiboot_module_t *boot_get_module(uint32_t index);

void boot_params_print(void);

char *boot_parse_param(const char *param);

int boot_has_param(const char *param);

uint64_t boot_get_available_memory(void);

#endif
