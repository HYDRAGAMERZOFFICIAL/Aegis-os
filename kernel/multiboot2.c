#include "multiboot2.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

bool multiboot2_is_valid(const void *mbi, uint32_t magic) {
    if (!mbi) return false;
    
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        return false;
    }
    
    const multiboot2_info_t *info = (const multiboot2_info_t *)mbi;
    if (info->total_size < sizeof(multiboot2_info_t)) {
        return false;
    }
    
    return true;
}

static const multiboot2_tag_t *multiboot2_find_tag(const void *mbi, uint32_t type) {
    if (!mbi) return NULL;
    
    const multiboot2_info_t *info = (const multiboot2_info_t *)mbi;
    const uint8_t *base = (const uint8_t *)mbi;
    size_t offset = 8;
    
    while (offset < info->total_size) {
        const multiboot2_tag_t *tag = (const multiboot2_tag_t *)(base + offset);
        
        if (tag->type == MULTIBOOT_HEADER_TAG_END) {
            break;
        }
        
        if (tag->type == type) {
            return tag;
        }
        
        offset += (tag->size + 7) & ~7;
    }
    
    return NULL;
}

const multiboot2_memory_tag_t *multiboot2_get_memory_info(const void *mbi) {
    if (!mbi) return NULL;
    return (const multiboot2_memory_tag_t *)multiboot2_find_tag(mbi, 4);
}

const multiboot2_module_tag_t *multiboot2_get_first_module(const void *mbi) {
    if (!mbi) return NULL;
    
    const multiboot2_info_t *info = (const multiboot2_info_t *)mbi;
    const uint8_t *base = (const uint8_t *)mbi;
    size_t offset = 8;
    
    while (offset < info->total_size) {
        const multiboot2_tag_t *tag = (const multiboot2_tag_t *)(base + offset);
        
        if (tag->type == MULTIBOOT_HEADER_TAG_END) {
            break;
        }
        
        if (tag->type == 3) {
            return (const multiboot2_module_tag_t *)tag;
        }
        
        offset += (tag->size + 7) & ~7;
    }
    
    return NULL;
}

const multiboot2_module_tag_t *multiboot2_get_next_module(const void *mbi, const multiboot2_module_tag_t *current) {
    if (!mbi || !current) return NULL;
    
    const multiboot2_info_t *info = (const multiboot2_info_t *)mbi;
    const uint8_t *base = (const uint8_t *)mbi;
    const uint8_t *current_ptr = (const uint8_t *)current;
    
    size_t offset = (current_ptr - base) + ((current->tag.size + 7) & ~7);
    
    while (offset < info->total_size) {
        const multiboot2_tag_t *tag = (const multiboot2_tag_t *)(base + offset);
        
        if (tag->type == MULTIBOOT_HEADER_TAG_END) {
            break;
        }
        
        if (tag->type == 3) {
            return (const multiboot2_module_tag_t *)tag;
        }
        
        offset += (tag->size + 7) & ~7;
    }
    
    return NULL;
}

const char *multiboot2_get_bootloader_name(const void *mbi) {
    if (!mbi) return NULL;
    
    const multiboot2_string_tag_t *tag = (const multiboot2_string_tag_t *)multiboot2_find_tag(mbi, 2);
    return tag ? tag->string : NULL;
}

const char *multiboot2_get_boot_cmdline(const void *mbi) {
    if (!mbi) return NULL;
    
    const multiboot2_string_tag_t *tag = (const multiboot2_string_tag_t *)multiboot2_find_tag(mbi, 1);
    return tag ? tag->string : NULL;
}

const multiboot2_framebuffer_info_tag_t *multiboot2_get_framebuffer_info(const void *mbi) {
    if (!mbi) return NULL;
    
    return (const multiboot2_framebuffer_info_tag_t *)multiboot2_find_tag(mbi, 8);
}

const multiboot2_acpi_tag_t *multiboot2_get_acpi_info(const void *mbi) {
    if (!mbi) return NULL;
    
    return (const multiboot2_acpi_tag_t *)multiboot2_find_tag(mbi, 14);
}

const multiboot2_efi64_tag_t *multiboot2_get_efi64_info(const void *mbi) {
    if (!mbi) return NULL;
    
    return (const multiboot2_efi64_tag_t *)multiboot2_find_tag(mbi, 12);
}

int multiboot2_validate_modules(const void *mbi) {
    if (!mbi) return -1;
    
    int valid_count = 0;
    const multiboot2_module_tag_t *module = multiboot2_get_first_module(mbi);
    
    while (module) {
        if (module->mod_start > 0 && module->mod_end > module->mod_start) {
            valid_count++;
        }
        module = multiboot2_get_next_module(mbi, module);
    }
    
    return valid_count;
}

int multiboot2_get_module_count(const void *mbi) {
    if (!mbi) return 0;
    
    int count = 0;
    const multiboot2_module_tag_t *module = multiboot2_get_first_module(mbi);
    
    while (module) {
        count++;
        module = multiboot2_get_next_module(mbi, module);
    }
    
    return count;
}
