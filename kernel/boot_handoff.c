#include "boot_handoff.h"
#include "multiboot2.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

boot_handoff_t *boot_create_handoff(void) {
    boot_handoff_t *handoff = malloc(sizeof(boot_handoff_t));
    if (!handoff) return NULL;
    
    memset(handoff, 0, sizeof(boot_handoff_t));
    handoff->magic = BOOT_HANDOFF_MAGIC;
    handoff->version = BOOT_HANDOFF_VERSION;
    handoff->bootloader_type = BOOT_BOOTLOADER_CUSTOM;
    handoff->architecture = BOOT_ARCH_X86_64;
    handoff->api_version = 1;
    
    return handoff;
}

boot_status_t boot_validate_handoff(const boot_handoff_t *handoff) {
    if (!handoff) return BOOT_STATUS_INVALID_STATE;
    if (handoff->magic != BOOT_HANDOFF_MAGIC) return BOOT_STATUS_INVALID_MAGIC;
    if (handoff->version != BOOT_HANDOFF_VERSION) return BOOT_STATUS_INVALID_VERSION;
    if (!boot_is_valid_memory_layout(handoff)) return BOOT_STATUS_MEMORY_ERROR;
    if (!boot_validate_modules(handoff)) return BOOT_STATUS_MODULE_ERROR;
    
    return BOOT_STATUS_SUCCESS;
}

boot_status_t boot_validate_multiboot2(const void *mbi, uint32_t magic) {
    if (!mbi) return BOOT_STATUS_INVALID_STATE;
    if (!multiboot2_is_valid(mbi, magic)) return BOOT_STATUS_INVALID_MAGIC;
    
    return BOOT_STATUS_SUCCESS;
}

boot_status_t boot_parse_multiboot2(const void *mbi, uint32_t magic, boot_handoff_t *handoff) {
    if (!mbi || !handoff) return BOOT_STATUS_INVALID_STATE;
    
    boot_status_t status = boot_validate_multiboot2(mbi, magic);
    if (status != BOOT_STATUS_SUCCESS) return status;
    
    handoff->bootloader_type = BOOT_BOOTLOADER_MULTIBOOT;
    handoff->architecture = BOOT_ARCH_X86_64;
    
    const multiboot2_memory_tag_t *mem_tag = multiboot2_get_memory_info(mbi);
    if (mem_tag) {
        handoff->memory_region_count = 0;
        
        const uint8_t *base = (const uint8_t *)mbi;
        const multiboot2_info_t *info = (const multiboot2_info_t *)mbi;
        size_t offset = 8;
        
        while (offset < info->total_size && handoff->memory_region_count < BOOT_MAX_MEMORY_REGIONS) {
            const multiboot2_tag_t *tag = (const multiboot2_tag_t *)(base + offset);
            
            if (tag->type == 6) {
                const multiboot2_mmap_tag_t *mmap = (const multiboot2_mmap_tag_t *)tag;
                size_t entry_count = (tag->size - sizeof(multiboot2_tag_t) - 8) / mmap->entry_size;
                
                for (size_t i = 0; i < entry_count && handoff->memory_region_count < BOOT_MAX_MEMORY_REGIONS; i++) {
                    handoff->memory_regions[handoff->memory_region_count].address = mmap->entries[i].address;
                    handoff->memory_regions[handoff->memory_region_count].size = mmap->entries[i].length;
                    handoff->memory_regions[handoff->memory_region_count].type = mmap->entries[i].type;
                    handoff->memory_region_count++;
                }
            }
            
            offset += (tag->size + 7) & ~7;
        }
    }
    
    int module_count = multiboot2_get_module_count(mbi);
    handoff->boot_module_count = 0;
    
    const multiboot2_module_tag_t *module = multiboot2_get_first_module(mbi);
    while (module && handoff->boot_module_count < BOOT_MAX_BOOT_MODULES) {
        handoff->boot_modules[handoff->boot_module_count].address = module->mod_start;
        handoff->boot_modules[handoff->boot_module_count].size = module->mod_end - module->mod_start;
        strncpy(handoff->boot_modules[handoff->boot_module_count].name,
               module->string, 255);
        handoff->boot_module_count++;
        
        module = multiboot2_get_next_module(mbi, module);
    }
    
    const char *cmdline = multiboot2_get_boot_cmdline(mbi);
    if (cmdline) {
        strncpy(handoff->boot_cmdline, cmdline, 1023);
    }
    
    const multiboot2_framebuffer_info_tag_t *fb = multiboot2_get_framebuffer_info(mbi);
    if (fb) {
        uint64_t fb_addr = ((uint64_t)fb->framebuffer_addr_high << 32) | fb->framebuffer_addr_low;
        boot_set_framebuffer_info(handoff, fb_addr, fb->framebuffer_width,
                                 fb->framebuffer_height, fb->framebuffer_pitch, fb->framebuffer_bpp);
    }
    
    const multiboot2_acpi_tag_t *acpi = multiboot2_get_acpi_info(mbi);
    if (acpi) {
        boot_set_acpi_info(handoff, acpi->rsdp_addr, 2);
    }
    
    return BOOT_STATUS_SUCCESS;
}

boot_status_t boot_validate_arm(const void *arm_info) {
    if (!arm_info) return BOOT_STATUS_INVALID_STATE;
    return BOOT_STATUS_SUCCESS;
}

boot_status_t boot_parse_arm_boot(const void *arm_info, boot_handoff_t *handoff) {
    if (!arm_info || !handoff) return BOOT_STATUS_INVALID_STATE;
    
    handoff->bootloader_type = BOOT_BOOTLOADER_UBOOT;
    handoff->architecture = BOOT_ARCH_ARM64;
    
    return BOOT_STATUS_SUCCESS;
}

int boot_add_memory_region(boot_handoff_t *handoff, uint64_t addr, uint64_t size, uint32_t type) {
    if (!handoff || handoff->memory_region_count >= BOOT_MAX_MEMORY_REGIONS) return -1;
    
    handoff->memory_regions[handoff->memory_region_count].address = addr;
    handoff->memory_regions[handoff->memory_region_count].size = size;
    handoff->memory_regions[handoff->memory_region_count].type = type;
    
    return handoff->memory_region_count++;
}

int boot_add_module(boot_handoff_t *handoff, uint64_t addr, uint64_t size,
                    const char *name, const char *cmdline) {
    if (!handoff || handoff->boot_module_count >= BOOT_MAX_BOOT_MODULES) return -1;
    
    boot_module_t *module = &handoff->boot_modules[handoff->boot_module_count];
    module->address = addr;
    module->size = size;
    
    if (name) strncpy(module->name, name, 255);
    if (cmdline) strncpy(module->cmdline, cmdline, 511);
    
    return handoff->boot_module_count++;
}

int boot_set_framebuffer_info(boot_handoff_t *handoff, uint64_t addr, uint32_t width,
                              uint32_t height, uint32_t pitch, uint8_t bpp) {
    if (!handoff) return -1;
    
    handoff->framebuffer.address = addr;
    handoff->framebuffer.width = width;
    handoff->framebuffer.height = height;
    handoff->framebuffer.pitch = pitch;
    handoff->framebuffer.bpp = bpp;
    handoff->framebuffer_count = 1;
    
    return 0;
}

int boot_set_acpi_info(boot_handoff_t *handoff, uint64_t rsdp_addr, uint32_t revision) {
    if (!handoff) return -1;
    
    handoff->acpi_info.rsdp_address = rsdp_addr;
    handoff->acpi_info.revision = revision;
    handoff->acpi_info.valid = true;
    
    return 0;
}

int boot_set_device_tree(boot_handoff_t *handoff, uint64_t dtb_addr, uint32_t dtb_size) {
    if (!handoff) return -1;
    
    handoff->device_info.device_tree_address = dtb_addr;
    handoff->device_info.device_tree_size = dtb_size;
    handoff->device_info.device_tree_valid = true;
    
    return 0;
}

bool boot_is_valid_memory_layout(const boot_handoff_t *handoff) {
    if (!handoff || handoff->memory_region_count == 0) return false;
    
    for (uint32_t i = 0; i < handoff->memory_region_count; i++) {
        if (handoff->memory_regions[i].size == 0) return false;
    }
    
    return true;
}

bool boot_validate_modules(const boot_handoff_t *handoff) {
    if (!handoff) return false;
    
    for (uint32_t i = 0; i < handoff->boot_module_count; i++) {
        if (handoff->boot_modules[i].address == 0 || handoff->boot_modules[i].size == 0) {
            return false;
        }
    }
    
    return true;
}

bool boot_has_framebuffer(const boot_handoff_t *handoff) {
    return handoff && handoff->framebuffer_count > 0;
}

bool boot_has_acpi(const boot_handoff_t *handoff) {
    return handoff && handoff->acpi_info.valid;
}

bool boot_has_device_tree(const boot_handoff_t *handoff) {
    return handoff && handoff->device_info.device_tree_valid;
}

uint32_t boot_calculate_checksum(boot_handoff_t *handoff) {
    if (!handoff) return 0;
    
    uint32_t checksum = 0;
    uint8_t *ptr = (uint8_t *)handoff;
    
    for (size_t i = 0; i < sizeof(boot_handoff_t) - sizeof(uint32_t); i++) {
        checksum += ptr[i];
    }
    
    return checksum;
}

void boot_dump_handoff_info(const boot_handoff_t *handoff) {
    if (!handoff) return;
    
    printf("Boot Handoff Information:\n");
    printf("  Magic: 0x%x\n", handoff->magic);
    printf("  Version: %u\n", handoff->version);
    printf("  Bootloader: %u\n", handoff->bootloader_type);
    printf("  Architecture: %u\n", handoff->architecture);
    printf("  Memory Regions: %u\n", handoff->memory_region_count);
    printf("  Boot Modules: %u\n", handoff->boot_module_count);
    printf("  Has Framebuffer: %s\n", boot_has_framebuffer(handoff) ? "yes" : "no");
    printf("  Has ACPI: %s\n", boot_has_acpi(handoff) ? "yes" : "no");
    printf("  Has Device Tree: %s\n", boot_has_device_tree(handoff) ? "yes" : "no");
}

void boot_free_handoff(boot_handoff_t *handoff) {
    free(handoff);
}
