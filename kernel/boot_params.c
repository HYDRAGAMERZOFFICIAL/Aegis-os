#include <kernel/boot_params.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    uint32_t mem_lower;
    uint32_t mem_upper;
    char bootloader_name[256];
    uint32_t mmap_count;
    multiboot_memory_map_t mmap[64];
    uint32_t modules_count;
    multiboot_module_t modules[32];
    char cmdline[512];
} boot_info_t;

static boot_info_t boot_info = {0};

void boot_params_init(void *multiboot_info_ptr)
{
    if (!multiboot_info_ptr) {
        return;
    }
    
    multiboot_info_t *info = (multiboot_info_t *)multiboot_info_ptr;
    
    if (info->flags & MULTIBOOT_INFO_MEMORY) {
        boot_info.mem_lower = info->mem_lower;
        boot_info.mem_upper = info->mem_upper;
    }
    
    if (info->flags & MULTIBOOT_INFO_BOOTLOADER_NAME) {
        strncpy(boot_info.bootloader_name, (const char *)info->boot_loader_name, 
                sizeof(boot_info.bootloader_name) - 1);
    }
    
    if (info->flags & MULTIBOOT_INFO_MEM_MAP) {
        multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)info->mmap_addr;
        boot_info.mmap_count = 0;
        
        for (uint32_t i = 0; i < info->mmap_length && boot_info.mmap_count < 64; 
             i += mmap->size + sizeof(mmap->size)) {
            memcpy(&boot_info.mmap[boot_info.mmap_count], mmap, sizeof(multiboot_memory_map_t));
            boot_info.mmap_count++;
            mmap = (multiboot_memory_map_t *)((uint8_t *)mmap + mmap->size + sizeof(mmap->size));
        }
    }
    
    if (info->flags & MULTIBOOT_INFO_MODULES) {
        multiboot_module_t *modules = (multiboot_module_t *)info->mods_addr;
        boot_info.modules_count = info->mods_count < 32 ? info->mods_count : 32;
        
        for (uint32_t i = 0; i < boot_info.modules_count; i++) {
            memcpy(&boot_info.modules[i], &modules[i], sizeof(multiboot_module_t));
        }
    }
    
    if (info->flags & MULTIBOOT_INFO_CMDLINE) {
        strncpy(boot_info.cmdline, (const char *)info->cmdline, 
                sizeof(boot_info.cmdline) - 1);
    }
}

uint32_t boot_get_mem_lower(void)
{
    return boot_info.mem_lower;
}

uint32_t boot_get_mem_upper(void)
{
    return boot_info.mem_upper;
}

uint64_t boot_get_total_memory(void)
{
    return (boot_info.mem_lower + boot_info.mem_upper) * 1024;
}

const char *boot_get_bootloader_name(void)
{
    return boot_info.bootloader_name;
}

const char *boot_get_cmdline(void)
{
    return boot_info.cmdline;
}

uint32_t boot_get_mmap_count(void)
{
    return boot_info.mmap_count;
}

const multiboot_memory_map_t *boot_get_mmap(uint32_t index)
{
    if (index >= boot_info.mmap_count) {
        return NULL;
    }
    
    return &boot_info.mmap[index];
}

uint32_t boot_get_modules_count(void)
{
    return boot_info.modules_count;
}

const multiboot_module_t *boot_get_module(uint32_t index)
{
    if (index >= boot_info.modules_count) {
        return NULL;
    }
    
    return &boot_info.modules[index];
}

void boot_params_print(void)
{
    printf("\n=== Boot Parameters ===\n");
    printf("Bootloader: %s\n", boot_info.bootloader_name);
    printf("Memory: %u KB lower, %u KB upper\n", boot_info.mem_lower, boot_info.mem_upper);
    printf("Total Memory: %llu MB\n", boot_get_total_memory() / 1024 / 1024);
    printf("Command Line: %s\n", boot_info.cmdline);
    printf("Memory Map Entries: %u\n", boot_info.mmap_count);
    printf("Loaded Modules: %u\n", boot_info.modules_count);
    
    printf("\nMemory Map:\n");
    printf("Address          | Size            | Type\n");
    printf("-----------------|-----------------|-----\n");
    
    for (uint32_t i = 0; i < boot_info.mmap_count; i++) {
        const multiboot_memory_map_t *mmap = &boot_info.mmap[i];
        const char *type_str = mmap->type == 1 ? "Available" : "Reserved";
        
        printf("0x%016llx | 0x%015llx | %s\n", 
               mmap->addr, mmap->len, type_str);
    }
}

char *boot_parse_param(const char *param)
{
    if (!param || !boot_info.cmdline) {
        return NULL;
    }
    
    char *cmdline_copy = strdup(boot_info.cmdline);
    if (!cmdline_copy) {
        return NULL;
    }
    
    char *saveptr = NULL;
    char *token = strtok_r(cmdline_copy, " ", &saveptr);
    
    while (token) {
        if (strstr(token, param) == token) {
            char *result = strchr(token, '=');
            if (result) {
                result++;
                char *value = strdup(result);
                free(cmdline_copy);
                return value;
            }
        }
        token = strtok_r(NULL, " ", &saveptr);
    }
    
    free(cmdline_copy);
    return NULL;
}

int boot_has_param(const char *param)
{
    if (!param || !boot_info.cmdline) {
        return 0;
    }
    
    return strstr(boot_info.cmdline, param) != NULL;
}

uint64_t boot_get_available_memory(void)
{
    uint64_t available = 0;
    
    for (uint32_t i = 0; i < boot_info.mmap_count; i++) {
        if (boot_info.mmap[i].type == MULTIBOOT_MEMORY_AVAILABLE) {
            available += boot_info.mmap[i].len;
        }
    }
    
    return available;
}
