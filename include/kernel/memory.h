#ifndef AEGIS_KERNEL_MEMORY_H
#define AEGIS_KERNEL_MEMORY_H

#include <kernel/types.h>

typedef enum {
    PROT_NONE = 0,
    PROT_READ = 1,
    PROT_WRITE = 2,
    PROT_EXEC = 4
} prot_flags_t;

typedef enum {
    PAGE_FLAG_PRESENT = (1 << 0),
    PAGE_FLAG_WRITABLE = (1 << 1),
    PAGE_FLAG_USER = (1 << 2),
    PAGE_FLAG_DIRTY = (1 << 3),
    PAGE_FLAG_ACCESSED = (1 << 4),
    PAGE_FLAG_ENCRYPTED = (1 << 5)
} page_flags_t;

typedef struct {
    u64 phys_addr;
    u64 flags;
    u32 ref_count;
} page_info_t;

typedef struct {
    u64 virt_addr;
    u64 phys_addr;
    u64 size;
    prot_flags_t prot;
    bool encrypted;
} vma_t;

typedef struct {
    u64 pid;
    void *page_table;
    vma_t *vmas;
    u32 vma_count;
    u64 heap_start;
    u64 heap_end;
    u64 stack_start;
    u64 stack_end;
} address_space_t;

int mmgr_init(void);
void *mmgr_alloc_page(void);
void mmgr_free_page(void *page);
void *mmgr_alloc_pages(u32 count);
void mmgr_free_pages(void *pages, u32 count);
void *mmgr_alloc_kernel_pages(u32 count);
void mmgr_free_kernel_pages(void *pages, u32 count);
int mmgr_map_pages(address_space_t *as, u64 virt_addr, u64 phys_addr, u32 count, prot_flags_t prot);
int mmgr_unmap_pages(address_space_t *as, u64 virt_addr, u32 count);
int mmgr_change_protection(address_space_t *as, u64 virt_addr, u32 count, prot_flags_t prot);
address_space_t *mmgr_create_address_space(void);
void mmgr_destroy_address_space(address_space_t *as);
int mmgr_enable_aslr(address_space_t *as);
int mmgr_encrypt_page(void *page);
int mmgr_decrypt_page(void *page);
u64 mmgr_get_phys_addr(address_space_t *as, u64 virt_addr);
int mmgr_secure_zero(void *ptr, size_t size);

#endif
