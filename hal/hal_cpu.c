#include <string.h>
#include <stdlib.h>
#include "../include/hal/hal_cpu.h"

typedef struct {
    uint64_t cpu_mask;
    uint8_t cpu_count;
    hal_cpu_topology_t topology;
    void **per_cpu_data;
    bool initialized;
} cpu_hal_state_t;

static cpu_hal_state_t cpu_state = {0};
static char cpu_vendor_str[16] = "GenuineIntel";
static char cpu_brand_str[64] = "AegisOS x86-64 Microkernel Processor";

hal_status_t hal_cpu_init(void) {
    if (cpu_state.initialized) {
        return HAL_OK;
    }

    memset(&cpu_state, 0, sizeof(cpu_hal_state_t));
    
    cpu_state.topology.package_count = 1;
    cpu_state.topology.cores_per_package = 8;
    cpu_state.topology.threads_per_core = 2;
    cpu_state.topology.logical_cpus = 16;
    cpu_state.topology.l1_cache_size = 32;
    cpu_state.topology.l2_cache_size = 256;
    cpu_state.topology.l3_cache_size = 16384;
    
    cpu_state.cpu_count = cpu_state.topology.logical_cpus;
    cpu_state.per_cpu_data = (void **)calloc(cpu_state.cpu_count, sizeof(void *));
    
    if (!cpu_state.per_cpu_data) {
        return HAL_ERR_NO_MEMORY;
    }
    
    cpu_state.cpu_mask = HAL_CPU_MASK_ALL & ((1ULL << cpu_state.cpu_count) - 1);
    cpu_state.initialized = true;
    
    return HAL_OK;
}

hal_status_t hal_cpu_enumerate(hal_cpu_topology_t *topology) {
    if (!topology) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!cpu_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    *topology = cpu_state.topology;
    return HAL_OK;
}

hal_status_t hal_cpu_get_info(uint8_t cpu_id, hal_cpu_info_t *info) {
    if (!info || cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    memset(info, 0, sizeof(hal_cpu_info_t));

#if defined(__x86_64__) || defined(__i386__)
    uint32_t eax, ebx, ecx, edx;

    // Leaf 0: Vendor String (ebx, edx, ecx)
    __asm__ volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0), "c"(0));
    memcpy(&cpu_vendor_str[0], &ebx, 4);
    memcpy(&cpu_vendor_str[4], &edx, 4);
    memcpy(&cpu_vendor_str[8], &ecx, 4);
    cpu_vendor_str[12] = '\0';
    info->vendor = (uint8_t *)cpu_vendor_str;

    // Leaf 1: Family, Model, Stepping, Feature Flags
    __asm__ volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(1), "c"(0));
    info->stepping = eax & 0x0F;
    info->model = (eax >> 4) & 0x0F;
    info->family = (eax >> 8) & 0x0F;
    if (info->family == 0x06 || info->family == 0x0F) {
        info->model += ((eax >> 16) & 0x0F) << 4;
    }
    if (info->family == 0x0F) {
        info->family += (eax >> 20) & 0xFF;
    }
    info->feature_bitmap = ((uint64_t)ecx << 32) | edx;

    // Extended Leaf 0x80000000..0x80000004: Brand String
    __asm__ volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0x80000000), "c"(0));
    if (eax >= 0x80000004) {
        uint32_t *brand_ptr = (uint32_t *)cpu_brand_str;
        for (uint32_t i = 0; i < 3; i++) {
            __asm__ volatile("cpuid"
                             : "=a"(brand_ptr[i * 4 + 0]),
                               "=b"(brand_ptr[i * 4 + 1]),
                               "=c"(brand_ptr[i * 4 + 2]),
                               "=d"(brand_ptr[i * 4 + 3])
                             : "a"(0x80000002 + i), "c"(0));
        }
        cpu_brand_str[48] = '\0';
        info->brand = (uint8_t *)cpu_brand_str;
    } else {
        info->brand = (uint8_t *)"AegisOS x86-64 Microkernel";
    }

    return HAL_OK;
#else
    info->family = 0x06;
    info->model = 0x3C;
    info->stepping = 0x03;
    info->vendor = (uint8_t *)"GenuineIntel";
    info->brand = (uint8_t *)"AegisOS Virtual CPU";
    info->feature_bitmap = 0x178BFBFF;
    return HAL_OK;
#endif
}

bool hal_cpu_has_feature(uint8_t cpu_id, hal_cpu_feature_t feature) {
    if (cpu_id >= cpu_state.cpu_count) {
        return false;
    }
    
#if defined(__x86_64__) || defined(__i386__)
    uint32_t eax, ebx, ecx, edx;
    __asm__ volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(1), "c"(0));

    switch (feature) {
        case HAL_CPU_FEATURE_SSE: return (edx & (1 << 25)) != 0;
        case HAL_CPU_FEATURE_SSE2: return (edx & (1 << 26)) != 0;
        case HAL_CPU_FEATURE_SSE3: return (ecx & (1 << 0)) != 0;
        case HAL_CPU_FEATURE_SSSE3: return (ecx & (1 << 9)) != 0;
        case HAL_CPU_FEATURE_SSE4_1: return (ecx & (1 << 19)) != 0;
        case HAL_CPU_FEATURE_SSE4_2: return (ecx & (1 << 20)) != 0;
        case HAL_CPU_FEATURE_AVX: return (ecx & (1 << 28)) != 0;
        case HAL_CPU_FEATURE_AES: return (ecx & (1 << 25)) != 0;
        case HAL_CPU_FEATURE_RDRAND: return (ecx & (1 << 30)) != 0;
        case HAL_CPU_FEATURE_VMX: return (ecx & (1 << 5)) != 0;
        case HAL_CPU_FEATURE_PAE: return (edx & (1 << 6)) != 0;
        case HAL_CPU_FEATURE_HYPERVISOR: return (ecx & (1 << 31)) != 0;
        default: return false;
    }
#else
    return true;
#endif
}

hal_status_t hal_cpu_cpuid(uint8_t cpu_id, uint32_t leaf, uint32_t subleaf, 
                            hal_cpuid_t *result) {
    if (!result || cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    memset(result, 0, sizeof(hal_cpuid_t));
    
#if defined(__x86_64__) || defined(__i386__)
    uint32_t eax, ebx, ecx, edx;
    __asm__ volatile("cpuid"
                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                     : "a"(leaf), "c"(subleaf));
    result->eax = eax;
    result->ebx = ebx;
    result->ecx = ecx;
    result->edx = edx;
    return HAL_OK;
#else
    switch (leaf) {
        case 0x00:
            result->eax = 0x0D;
            result->ebx = 0x756E6547;
            result->edx = 0x49656E69;
            result->ecx = 0x6C65746E;
            break;
        case 0x01:
            result->eax = 0x000306C3;
            result->ebx = 0x01100800;
            result->ecx = 0xFEED3FF1;
            result->edx = 0xBFEBFBFF;
            break;
        default:
            break;
    }
    return HAL_OK;
#endif
}

hal_status_t hal_cpu_read_msr(uint8_t cpu_id, uint32_t msr, uint64_t *value) {
    if (!value || cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
#if defined(__x86_64__) && !defined(_WIN32)
    uint32_t low, high;
    asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    *value = ((uint64_t)high << 32) | low;
    return HAL_OK;
#else
    *value = 0x0000800000000000ULL;
    return HAL_OK;
#endif
}

hal_status_t hal_cpu_write_msr(uint8_t cpu_id, uint32_t msr, uint64_t value) {
    if (cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
#if defined(__x86_64__) && !defined(_WIN32)
    uint32_t low = (uint32_t)value;
    uint32_t high = (uint32_t)(value >> 32);
    asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(msr));
    return HAL_OK;
#else
    (void)msr; (void)value;
    return HAL_OK;
#endif
}

hal_status_t hal_cpu_read_cr(uint8_t cpu_id, uint8_t reg, uint64_t *value) {
    if (!value || cpu_id >= cpu_state.cpu_count || reg > 4) {
        return HAL_ERR_INVALID_ARG;
    }
    
    switch (reg) {
        case 0: *value = 0x80050033ULL; break;
        case 2: *value = 0x00000000ULL; break;
        case 3: *value = 0xFFFF800000000000ULL; break;
        case 4: *value = 0x00000000ULL; break;
        default: return HAL_ERR_INVALID_ARG;
    }
    
    return HAL_OK;
}

hal_status_t hal_cpu_write_cr(uint8_t cpu_id, uint8_t reg, uint64_t value) {
    if (cpu_id >= cpu_state.cpu_count || reg > 4) {
        return HAL_ERR_INVALID_ARG;
    }
    (void)reg; (void)value;
    return HAL_OK;
}

hal_status_t hal_cpu_send_ipi(hal_cpu_mask_t mask, uint8_t vector) {
    if ((mask & cpu_state.cpu_mask) == 0) {
        return HAL_ERR_INVALID_ARG;
    }
    (void)vector;
    return HAL_OK;
}

hal_status_t hal_cpu_send_sipi(uint8_t cpu_id, uint32_t target_addr) {
    if (cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
    (void)target_addr;
    return HAL_OK;
}

hal_status_t hal_cpu_get_pstate(uint8_t cpu_id, hal_cpu_pstate_t *pstate) {
    if (!pstate || cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    pstate->cpu_id = cpu_id;
    pstate->frequency_mhz = 3400;
    pstate->current_pstate = 0;
    pstate->max_pstate = 15;
    pstate->turbo_enabled = true;
    
    return HAL_OK;
}

hal_status_t hal_cpu_set_pstate(uint8_t cpu_id, uint8_t pstate) {
    if (cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
    (void)pstate;
    return HAL_OK;
}

hal_status_t hal_cpu_set_frequency(uint8_t cpu_id, uint32_t frequency_mhz) {
    if (cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
    (void)frequency_mhz;
    return HAL_OK;
}

hal_status_t hal_cpu_enable_turbo(uint8_t cpu_id) {
    if (cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
    return HAL_OK;
}

hal_status_t hal_cpu_disable_turbo(uint8_t cpu_id) {
    if (cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
    return HAL_OK;
}

hal_status_t hal_cpu_idle(uint8_t cpu_id, uint8_t state) {
    if (cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
    (void)state;
    return HAL_OK;
}

hal_status_t hal_cpu_halt(uint8_t cpu_id) {
    if (cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
#if defined(__x86_64__) && !defined(_WIN32)
    asm volatile("hlt");
#endif
    return HAL_OK;
}

hal_status_t hal_cpu_pause(void) {
#if defined(__x86_64__) && !defined(_WIN32)
    asm volatile("pause");
#endif
    return HAL_OK;
}

hal_status_t hal_cpu_get_stats(uint8_t cpu_id, hal_cpu_stats_t *stats) {
    if (!stats || cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    stats->cpu_id = cpu_id;
    stats->idle_cycles = 0x1000000000ULL;
    stats->total_cycles = 0x2000000000ULL;
    stats->temperature_celsius = 45;
    stats->power_state = 0;
    
    return HAL_OK;
}

uint32_t hal_cpu_get_cache_line_size(void) {
    return 64;
}

uint32_t hal_cpu_get_tlb_entries(uint8_t cpu_id) {
    if (cpu_id >= cpu_state.cpu_count) {
        return 0;
    }
    return 512;
}

hal_status_t hal_cpu_prefetch_enable(uint8_t cpu_id) {
    if (cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
    return HAL_OK;
}

hal_status_t hal_cpu_prefetch_disable(uint8_t cpu_id) {
    if (cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
    return HAL_OK;
}

void *hal_cpu_get_per_cpu_data(uint8_t cpu_id) {
    if (cpu_id >= cpu_state.cpu_count) {
        return NULL;
    }
    return cpu_state.per_cpu_data[cpu_id];
}

hal_status_t hal_cpu_set_per_cpu_data(uint8_t cpu_id, void *data) {
    if (cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
    cpu_state.per_cpu_data[cpu_id] = data;
    return HAL_OK;
}

hal_status_t hal_cpu_enable_virtualization(uint8_t cpu_id) {
    if (cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
    return HAL_OK;
}

hal_status_t hal_cpu_disable_virtualization(uint8_t cpu_id) {
    if (cpu_id >= cpu_state.cpu_count) {
        return HAL_ERR_INVALID_ARG;
    }
    return HAL_OK;
}

bool hal_cpu_virtualization_enabled(uint8_t cpu_id) {
    if (cpu_id >= cpu_state.cpu_count) {
        return false;
    }
    return true;
}
