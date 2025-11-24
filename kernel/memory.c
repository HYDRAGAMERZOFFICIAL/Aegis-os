#include <kernel/memory.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    page_info_t *pages;
    u64 total_pages;
    u64 free_pages;
    u8 *page_bitmap;
    uint lock;
} mmgr_state_t;

static mmgr_state_t mmgr_state = {0};

int mmgr_init(void)
{
    mmgr_state.total_pages = 0x100000;
    mmgr_state.free_pages = mmgr_state.total_pages;
    mmgr_state.lock = 0;

    mmgr_state.pages = (page_info_t *)calloc(mmgr_state.total_pages, sizeof(page_info_t));
    if (!mmgr_state.pages) return -1;

    mmgr_state.page_bitmap = (u8 *)calloc((mmgr_state.total_pages + 7) / 8, sizeof(u8));
    if (!mmgr_state.page_bitmap) return -1;

    for (u64 i = 0; i < mmgr_state.total_pages; i++) {
        mmgr_state.pages[i].phys_addr = i * PAGE_SIZE;
        mmgr_state.pages[i].ref_count = 0;
        mmgr_state.pages[i].flags = 0;
    }

    return 0;
}

void *mmgr_alloc_page(void)
{
    for (u64 i = 0; i < mmgr_state.total_pages; i++) {
        u32 byte_idx = i / 8;
        u32 bit_idx = i % 8;

        if (!(mmgr_state.page_bitmap[byte_idx] & (1 << bit_idx))) {
            mmgr_state.page_bitmap[byte_idx] |= (1 << bit_idx);
            mmgr_state.pages[i].ref_count = 1;
            mmgr_state.free_pages--;
            return (void *)mmgr_state.pages[i].phys_addr;
        }
    }
    return NULL;
}

void mmgr_free_page(void *page)
{
    if (!page) return;

    u64 phys_addr = (u64)page;
    u64 page_num = phys_addr / PAGE_SIZE;

    if (page_num < mmgr_state.total_pages) {
        u32 byte_idx = page_num / 8;
        u32 bit_idx = page_num % 8;

        mmgr_state.page_bitmap[byte_idx] &= ~(1 << bit_idx);
        mmgr_state.pages[page_num].ref_count = 0;
        mmgr_state.free_pages++;
    }
}

void *mmgr_alloc_pages(u32 count)
{
    void *first_page = NULL;
    for (u32 i = 0; i < count; i++) {
        void *page = mmgr_alloc_page();
        if (!page) {
            for (u32 j = 0; j < i; j++) {
                mmgr_free_page(page);
            }
            return NULL;
        }
        if (i == 0) first_page = page;
    }
    return first_page;
}

void mmgr_free_pages(void *pages, u32 count)
{
    u64 phys_addr = (u64)pages;
    for (u32 i = 0; i < count; i++) {
        mmgr_free_page((void *)(phys_addr + (i * PAGE_SIZE)));
    }
}

void *mmgr_alloc_kernel_pages(u32 count)
{
    return mmgr_alloc_pages(count);
}

void mmgr_free_kernel_pages(void *pages, u32 count)
{
    mmgr_free_pages(pages, count);
}

int mmgr_map_pages(address_space_t *as, u64 virt_addr, u64 phys_addr, u32 count, prot_flags_t prot)
{
    if (!as) return -1;

    for (u32 i = 0; i < count; i++) {
        vma_t *vma = NULL;
        for (u32 j = 0; j < as->vma_count; j++) {
            if (as->vmas[j].virt_addr == virt_addr + (i * PAGE_SIZE)) {
                vma = &as->vmas[j];
                break;
            }
        }

        if (!vma && as->vma_count < 1024) {
            vma = &as->vmas[as->vma_count++];
        }

        if (vma) {
            vma->virt_addr = virt_addr + (i * PAGE_SIZE);
            vma->phys_addr = phys_addr + (i * PAGE_SIZE);
            vma->size = PAGE_SIZE;
            vma->prot = prot;
            vma->encrypted = false;
        }
    }

    return 0;
}

int mmgr_unmap_pages(address_space_t *as, u64 virt_addr, u32 count)
{
    if (!as) return -1;

    for (u32 i = 0; i < count; i++) {
        for (u32 j = 0; j < as->vma_count; j++) {
            if (as->vmas[j].virt_addr == virt_addr + (i * PAGE_SIZE)) {
                for (u32 k = j; k < as->vma_count - 1; k++) {
                    as->vmas[k] = as->vmas[k + 1];
                }
                as->vma_count--;
                break;
            }
        }
    }

    return 0;
}

int mmgr_change_protection(address_space_t *as, u64 virt_addr, u32 count, prot_flags_t prot)
{
    if (!as) return -1;

    for (u32 i = 0; i < count; i++) {
        for (u32 j = 0; j < as->vma_count; j++) {
            if (as->vmas[j].virt_addr == virt_addr + (i * PAGE_SIZE)) {
                as->vmas[j].prot = prot;
                break;
            }
        }
    }

    return 0;
}

address_space_t *mmgr_create_address_space(void)
{
    address_space_t *as = (address_space_t *)malloc(sizeof(address_space_t));
    if (!as) return NULL;

    as->vmas = (vma_t *)malloc(1024 * sizeof(vma_t));
    if (!as->vmas) {
        free(as);
        return NULL;
    }

    as->vma_count = 0;
    as->heap_start = KERNEL_HEAP_BASE;
    as->heap_end = KERNEL_HEAP_BASE;
    as->stack_start = KERNEL_BASE - 0x1000;
    as->stack_end = KERNEL_BASE;

    return as;
}

void mmgr_destroy_address_space(address_space_t *as)
{
    if (!as) return;
    free(as->vmas);
    free(as);
}

int mmgr_enable_aslr(address_space_t *as)
{
    if (!as) return -1;
    for (u32 i = 0; i < as->vma_count; i++) {
        as->vmas[i].virt_addr += (rand() & 0xFFF000);
    }
    return 0;
}

int mmgr_encrypt_page(void *page)
{
    return 0;
}

int mmgr_decrypt_page(void *page)
{
    return 0;
}

u64 mmgr_get_phys_addr(address_space_t *as, u64 virt_addr)
{
    if (!as) return 0;

    for (u32 i = 0; i < as->vma_count; i++) {
        if (as->vmas[i].virt_addr == virt_addr) {
            return as->vmas[i].phys_addr;
        }
    }

    return 0;
}

int mmgr_secure_zero(void *ptr, size_t size)
{
    volatile u8 *p = (volatile u8 *)ptr;
    for (size_t i = 0; i < size; i++) {
        p[i] = 0;
    }
    return 0;
}
