#ifndef AEGIS_HAL_CPU_H
#define AEGIS_HAL_CPU_H

#include <stdint.h>
#include <stdbool.h>
#include "hal.h"

typedef struct {
    uint8_t package_count;
    uint8_t cores_per_package;
    uint8_t threads_per_core;
    uint16_t logical_cpus;
    uint16_t l1_cache_size;
    uint16_t l2_cache_size;
    uint32_t l3_cache_size;
} hal_cpu_topology_t;

typedef enum {
    HAL_CPU_FEATURE_SSE = 1,
    HAL_CPU_FEATURE_SSE2 = 2,
    HAL_CPU_FEATURE_SSE3 = 3,
    HAL_CPU_FEATURE_SSSE3 = 4,
    HAL_CPU_FEATURE_SSE4_1 = 5,
    HAL_CPU_FEATURE_SSE4_2 = 6,
    HAL_CPU_FEATURE_AVX = 7,
    HAL_CPU_FEATURE_AVX2 = 8,
    HAL_CPU_FEATURE_AVX512 = 9,
    HAL_CPU_FEATURE_AES = 10,
    HAL_CPU_FEATURE_SHA = 11,
    HAL_CPU_FEATURE_RDRAND = 12,
    HAL_CPU_FEATURE_VMX = 13,
    HAL_CPU_FEATURE_SVM = 14,
    HAL_CPU_FEATURE_SMX = 15,
    HAL_CPU_FEATURE_PAE = 16,
    HAL_CPU_FEATURE_PSE36 = 17,
    HAL_CPU_FEATURE_HYPERVISOR = 18,
    HAL_CPU_FEATURE_TSC_DEADLINE = 19,
} hal_cpu_feature_t;

typedef struct {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
} hal_cpuid_t;

typedef struct {
    uint8_t cpu_id;
    uint32_t frequency_mhz;
    uint8_t current_pstate;
    uint8_t max_pstate;
    bool turbo_enabled;
} hal_cpu_pstate_t;

typedef struct {
    uint8_t cpu_id;
    uint64_t idle_cycles;
    uint64_t total_cycles;
    uint32_t temperature_celsius;
    uint8_t power_state;
} hal_cpu_stats_t;

typedef struct {
    uint8_t *vendor;
    uint8_t *brand;
    uint8_t family;
    uint8_t model;
    uint8_t stepping;
    uint64_t feature_bitmap;
} hal_cpu_info_t;

typedef uint64_t hal_cpu_mask_t;

#define HAL_CPU_MASK_ALL (~0ULL)
#define HAL_CPU_MASK_NONE (0ULL)

hal_status_t hal_cpu_init(void);
hal_status_t hal_cpu_enumerate(hal_cpu_topology_t *topology);
hal_status_t hal_cpu_get_info(uint8_t cpu_id, hal_cpu_info_t *info);
bool hal_cpu_has_feature(uint8_t cpu_id, hal_cpu_feature_t feature);
hal_status_t hal_cpu_cpuid(uint8_t cpu_id, uint32_t leaf, uint32_t subleaf, hal_cpuid_t *result);

hal_status_t hal_cpu_read_msr(uint8_t cpu_id, uint32_t msr, uint64_t *value);
hal_status_t hal_cpu_write_msr(uint8_t cpu_id, uint32_t msr, uint64_t value);
hal_status_t hal_cpu_read_cr(uint8_t cpu_id, uint8_t reg, uint64_t *value);
hal_status_t hal_cpu_write_cr(uint8_t cpu_id, uint8_t reg, uint64_t value);

hal_status_t hal_cpu_send_ipi(hal_cpu_mask_t mask, uint8_t vector);
hal_status_t hal_cpu_send_sipi(uint8_t cpu_id, uint32_t target_addr);

hal_status_t hal_cpu_get_pstate(uint8_t cpu_id, hal_cpu_pstate_t *pstate);
hal_status_t hal_cpu_set_pstate(uint8_t cpu_id, uint8_t pstate);
hal_status_t hal_cpu_set_frequency(uint8_t cpu_id, uint32_t frequency_mhz);
hal_status_t hal_cpu_enable_turbo(uint8_t cpu_id);
hal_status_t hal_cpu_disable_turbo(uint8_t cpu_id);

hal_status_t hal_cpu_idle(uint8_t cpu_id, uint8_t state);
hal_status_t hal_cpu_halt(uint8_t cpu_id);
hal_status_t hal_cpu_pause(void);

hal_status_t hal_cpu_get_stats(uint8_t cpu_id, hal_cpu_stats_t *stats);
uint32_t hal_cpu_get_cache_line_size(void);
uint32_t hal_cpu_get_tlb_entries(uint8_t cpu_id);

hal_status_t hal_cpu_prefetch_enable(uint8_t cpu_id);
hal_status_t hal_cpu_prefetch_disable(uint8_t cpu_id);

void *hal_cpu_get_per_cpu_data(uint8_t cpu_id);
hal_status_t hal_cpu_set_per_cpu_data(uint8_t cpu_id, void *data);

hal_status_t hal_cpu_enable_virtualization(uint8_t cpu_id);
hal_status_t hal_cpu_disable_virtualization(uint8_t cpu_id);
bool hal_cpu_virtualization_enabled(uint8_t cpu_id);

#endif
