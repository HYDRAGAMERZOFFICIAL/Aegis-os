#ifndef AEGIS_BOOT_LIMINE_H
#define AEGIS_BOOT_LIMINE_H

#include <stdint.h>

/*
 * Limine Boot Protocol v2 Specification Header
 * Standard Higher-Half freestanding boot handoff protocol for x86-64.
 */

#define LIMINE_COMMON_MAGIC 0xc7b1dd30df4c8b88, 0x0a691e7ea38a283b

#define LIMINE_FRAMEBUFFER_REQUEST \
    { 0xc7b1dd30df4c8b88, 0x0a691e7ea38a283b, 0x9d5827dcd881dd7b, 0xa3148604f34c6e64 }

#define LIMINE_MEMMAP_REQUEST \
    { 0xc7b1dd30df4c8b88, 0x0a691e7ea38a283b, 0x67cf3d9d378a14b6, 0x8d50148827993ba7 }

#define LIMINE_KERNEL_ADDRESS_REQUEST \
    { 0xc7b1dd30df4c8b88, 0x0a691e7ea38a283b, 0x71ba76863cc55f63, 0xb2644a48c516a487 }

#define LIMINE_RSDP_REQUEST \
    { 0xc7b1dd30df4c8b88, 0x0a691e7ea38a283b, 0xc5e77b6b397e7b43, 0x276378314e96eabf }

#define LIMINE_BASE_REVISION(N) \
    uint64_t limine_base_revision[3] = { 0xf9562b2d5c95a6c8, 0x6a7b384944536bdc, (N) }

struct limine_framebuffer {
    void *address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp;
    uint8_t memory_model;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
    uint8_t unused[7];
    uint64_t edid_size;
    void *edid;
};

struct limine_framebuffer_response {
    uint64_t revision;
    uint64_t framebuffer_count;
    struct limine_framebuffer **framebuffers;
};

struct limine_framebuffer_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_framebuffer_response *response;
};

#define LIMINE_MEMMAP_USABLE                 0
#define LIMINE_MEMMAP_RESERVED               1
#define LIMINE_MEMMAP_ACPI_RECLAIMABLE       2
#define LIMINE_MEMMAP_ACPI_NVS               3
#define LIMINE_MEMMAP_BAD_MEMORY             4
#define LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE 5
#define LIMINE_MEMMAP_KERNEL_AND_MODULES     6
#define LIMINE_MEMMAP_FRAMEBUFFER            7

struct limine_memmap_entry {
    uint64_t base;
    uint64_t length;
    uint64_t type;
};

struct limine_memmap_response {
    uint64_t revision;
    uint64_t entry_count;
    struct limine_memmap_entry **entries;
};

struct limine_memmap_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_memmap_response *response;
};

struct limine_kernel_address_response {
    uint64_t revision;
    uint64_t physical_base;
    uint64_t virtual_base;
};

struct limine_kernel_address_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_kernel_address_response *response;
};

struct limine_rsdp_response {
    uint64_t revision;
    void *address;
};

struct limine_rsdp_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_rsdp_response *response;
};

#endif
