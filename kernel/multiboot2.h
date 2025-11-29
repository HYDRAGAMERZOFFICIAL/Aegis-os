#ifndef AEGIS_MULTIBOOT2_H
#define AEGIS_MULTIBOOT2_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MULTIBOOT2_MAGIC 0x36d76289
#define MULTIBOOT2_BOOTLOADER_MAGIC 0x2badb002
#define MULTIBOOT_ARCHITECTURE_I386 0
#define MULTIBOOT_HEADER_TAG_END 0
#define MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST 1
#define MULTIBOOT_HEADER_TAG_ADDRESS 2
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS 3
#define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS 4
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER 5
#define MULTIBOOT_HEADER_TAG_MODULE_ALIGN 6
#define MULTIBOOT_HEADER_TAG_EFI_BS 7
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI32 9
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI64 10
#define MULTIBOOT_HEADER_TAG_RELOCATABLE 10

#define MULTIBOOT_BOOTLOADER_MAGIC_EFI32 0x2badb002
#define MULTIBOOT_BOOTLOADER_MAGIC_EFI64 0x2badb002
#define MULTIBOOT_BOOTLOADER_MAGIC_UEFI 0xe85250d6

typedef struct {
    uint32_t magic;
    uint32_t arch;
    uint32_t header_length;
    uint32_t checksum;
} multiboot2_header_t;

typedef struct {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
} multiboot2_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t requests[10];
} multiboot2_information_request_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
} multiboot2_address_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t entry_addr;
} multiboot2_entry_address_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t console_flags;
} multiboot2_console_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
} multiboot2_framebuffer_tag_t;

typedef struct {
    uint32_t total_size;
    uint32_t reserved;
} multiboot2_info_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t console_control;
} multiboot2_bootloader_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t mem_lower;
    uint32_t mem_upper;
} multiboot2_memory_tag_t;

typedef struct {
    uint64_t address;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} multiboot2_mmap_entry_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t entry_size;
    uint32_t entry_version;
    multiboot2_mmap_entry_t entries[64];
} multiboot2_mmap_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    char string[];
} multiboot2_string_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t mod_start;
    uint32_t mod_end;
    char string[];
} multiboot2_module_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t drive_number;
    uint32_t drive_mode;
    uint32_t cylinders;
    uint32_t heads;
    uint32_t sectors;
    uint32_t ports_count;
    uint16_t ports[20];
} multiboot2_drive_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t ebda_addr;
} multiboot2_apm_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t vbe_mode;
    uint32_t vbe_interface_seg;
    uint32_t vbe_interface_off;
    uint32_t vbe_interface_len;
    uint64_t vbe_control_info;
    uint64_t vbe_mode_info;
} multiboot2_vbe_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t framebuffer_addr_low;
    uint32_t framebuffer_addr_high;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint8_t color_info;
} multiboot2_framebuffer_info_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint64_t rsdp_addr;
} multiboot2_acpi_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t smbios_version;
    uint32_t smbios_address;
    uint32_t smbios_length;
} multiboot2_smbios_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t size;
    uint32_t handle;
    uint32_t device_path[];
} multiboot2_efi_bootservices_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t pointer_size;
    uint32_t reserved;
} multiboot2_efi32_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t pointer_size;
    uint32_t reserved;
} multiboot2_efi64_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint64_t image_handle;
    uint64_t system_table;
} multiboot2_efi_system_table_tag_t;

typedef struct {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t reserved;
} multiboot2_efi_memory_descriptor_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t descriptor_size;
    uint32_t descriptor_version;
    multiboot2_efi_memory_descriptor_t descriptors[128];
} multiboot2_efi_mmap_tag_t;

typedef struct {
    multiboot2_tag_t tag;
    uint32_t load_base_addr;
} multiboot2_load_base_addr_tag_t;

bool multiboot2_is_valid(const void *mbi, uint32_t magic);
const multiboot2_memory_tag_t *multiboot2_get_memory_info(const void *mbi);
const multiboot2_module_tag_t *multiboot2_get_first_module(const void *mbi);
const multiboot2_module_tag_t *multiboot2_get_next_module(const void *mbi, const multiboot2_module_tag_t *current);
const char *multiboot2_get_bootloader_name(const void *mbi);
const char *multiboot2_get_boot_cmdline(const void *mbi);
const multiboot2_framebuffer_info_tag_t *multiboot2_get_framebuffer_info(const void *mbi);
const multiboot2_acpi_tag_t *multiboot2_get_acpi_info(const void *mbi);
const multiboot2_efi64_tag_t *multiboot2_get_efi64_info(const void *mbi);
int multiboot2_validate_modules(const void *mbi);
int multiboot2_get_module_count(const void *mbi);

#endif
