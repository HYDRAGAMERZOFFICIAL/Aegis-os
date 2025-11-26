#ifndef AEGIS_HAL_GPU_H
#define AEGIS_HAL_GPU_H

#include <stdint.h>
#include <stdbool.h>
#include "hal.h"

typedef enum {
    HAL_GPU_TYPE_UNKNOWN = 0,
    HAL_GPU_TYPE_NVIDIA,
    HAL_GPU_TYPE_AMD,
    HAL_GPU_TYPE_INTEL,
    HAL_GPU_TYPE_ARM_MALI,
    HAL_GPU_TYPE_QUALCOMM_ADRENO,
    HAL_GPU_TYPE_VIRTUAL,
} hal_gpu_type_t;

typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    hal_gpu_type_t type;
    uint8_t device_index;
    uint64_t vram_size;
    uint64_t reserved_vram;
    uint32_t sm_count;
    uint32_t clock_speed_mhz;
    uint8_t compute_capability_major;
    uint8_t compute_capability_minor;
    bool ecc_enabled;
    bool preemption_supported;
} hal_gpu_info_t;

typedef struct {
    uint64_t total_memory;
    uint64_t free_memory;
    uint64_t used_memory;
    uint32_t current_clock;
    uint32_t current_memory_clock;
    uint8_t temperature_celsius;
    uint8_t power_usage_percent;
} hal_gpu_stats_t;

typedef struct {
    uint64_t context_id;
    uint32_t process_id;
    uint64_t allocated_memory;
    bool is_active;
} hal_gpu_context_t;

typedef struct {
    uint64_t base_address;
    uint64_t size;
    uint32_t page_size;
    bool is_virtual;
} hal_gpu_memory_segment_t;

typedef enum {
    HAL_GPU_CACHE_L1 = 1,
    HAL_GPU_CACHE_L2 = 2,
} hal_gpu_cache_type_t;

typedef struct {
    uint64_t config_addr;
    uint64_t config_data;
    uint32_t mmio_size;
    uint32_t rom_size;
    uint64_t rom_base;
} hal_gpu_bar_info_t;

hal_status_t hal_gpu_init(void);
hal_status_t hal_gpu_enumerate(uint32_t *device_count);
hal_status_t hal_gpu_get_info(uint8_t device_id, hal_gpu_info_t *info);
hal_status_t hal_gpu_get_stats(uint8_t device_id, hal_gpu_stats_t *stats);

hal_status_t hal_gpu_create_context(uint8_t device_id, uint32_t process_id, 
                                     uint64_t *context_id);
hal_status_t hal_gpu_destroy_context(uint64_t context_id);
hal_status_t hal_gpu_get_context_info(uint64_t context_id, hal_gpu_context_t *info);

hal_status_t hal_gpu_allocate_memory(uint64_t context_id, uint64_t size, 
                                      uint64_t *gpu_address);
hal_status_t hal_gpu_free_memory(uint64_t gpu_address);
hal_status_t hal_gpu_copy_to_device(uint64_t context_id, const void *host_ptr, 
                                     uint64_t device_addr, uint64_t size);
hal_status_t hal_gpu_copy_to_host(uint64_t context_id, uint64_t device_addr, 
                                   void *host_ptr, uint64_t size);
hal_status_t hal_gpu_memset(uint64_t context_id, uint64_t device_addr, uint8_t value, 
                             uint64_t size);

hal_status_t hal_gpu_submit_kernel(uint64_t context_id, const void *kernel_code, 
                                    uint32_t code_size, uint32_t block_x, uint32_t block_y, 
                                    uint32_t block_z, uint32_t grid_x, uint32_t grid_y, 
                                    uint32_t grid_z);
hal_status_t hal_gpu_wait_for_completion(uint64_t context_id, uint32_t timeout_ms);
hal_status_t hal_gpu_get_last_error(uint64_t context_id, uint32_t *error_code);

hal_status_t hal_gpu_set_power_state(uint8_t device_id, uint8_t pstate);
hal_status_t hal_gpu_set_clock_frequency(uint8_t device_id, uint32_t frequency_mhz);
hal_status_t hal_gpu_enable_ecc(uint8_t device_id);
hal_status_t hal_gpu_disable_ecc(uint8_t device_id);

hal_status_t hal_gpu_flush_cache(uint64_t context_id, hal_gpu_cache_type_t cache_type);
hal_status_t hal_gpu_invalidate_cache(uint64_t context_id, hal_gpu_cache_type_t cache_type);
hal_status_t hal_gpu_set_cache_policy(uint64_t context_id, uint8_t policy);

hal_status_t hal_gpu_get_bar_info(uint8_t device_id, hal_gpu_bar_info_t *bar_info);
hal_status_t hal_gpu_map_bar_region(uint8_t device_id, uint32_t bar_index, 
                                     uint64_t offset, uint64_t size, void **vaddr);
hal_status_t hal_gpu_unmap_bar_region(void *vaddr);

hal_status_t hal_gpu_enable_preemption(uint64_t context_id);
hal_status_t hal_gpu_disable_preemption(uint64_t context_id);
hal_status_t hal_gpu_preempt_kernel(uint64_t context_id);

hal_status_t hal_gpu_reset_device(uint8_t device_id);
hal_status_t hal_gpu_is_healthy(uint8_t device_id, bool *healthy);

#endif
