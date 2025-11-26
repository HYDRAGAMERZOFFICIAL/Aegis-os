#include <string.h>
#include <stdlib.h>
#include "../include/hal/hal.h"
#include "../include/hal/hal_cpu.h"
#include "../include/hal/hal_firmware.h"
#include "../include/hal/hal_acpi_uefi.h"
#include "../include/hal/hal_gpu.h"
#include "../include/hal/hal_gpu_display.h"
#include "../include/hal/hal_audio.h"
#include "../include/hal/hal_storage.h"
#include "../include/hal/hal_usb.h"
#include "../include/hal/hal_power.h"

typedef struct {
    bool cpu_initialized;
    bool firmware_initialized;
    bool acpi_uefi_initialized;
    bool gpu_initialized;
    bool gpu_display_initialized;
    bool audio_initialized;
    bool storage_initialized;
    bool usb_initialized;
    bool power_initialized;
    bool iommu_enabled;
    hal_platform_t platform;
} hal_internal_state_t;

static hal_internal_state_t hal_int_state = {0};

hal_status_t hal_init(void) {
    hal_status_t status;
    
    memset(&hal_int_state, 0, sizeof(hal_internal_state_t));
    
    #if defined(__x86_64__)
    hal_int_state.platform = HAL_PLATFORM_X86_64;
    #elif defined(__i386__)
    hal_int_state.platform = HAL_PLATFORM_X86_32;
    #elif defined(__aarch64__)
    hal_int_state.platform = HAL_PLATFORM_ARM64;
    #elif defined(__arm__)
    hal_int_state.platform = HAL_PLATFORM_ARM32;
    #else
    hal_int_state.platform = HAL_PLATFORM_UNKNOWN;
    #endif
    
    status = hal_cpu_init();
    if (status != HAL_OK) {
        return status;
    }
    hal_int_state.cpu_initialized = true;
    
    status = hal_firmware_init();
    if (status != HAL_OK) {
        return status;
    }
    hal_int_state.firmware_initialized = true;
    
    status = hal_acpi_uefi_init(0x0000);
    if (status != HAL_OK) {
        return status;
    }
    hal_int_state.acpi_uefi_initialized = true;
    
    status = hal_gpu_init();
    if (status != HAL_OK) {
        return status;
    }
    hal_int_state.gpu_initialized = true;
    
    status = hal_gpu_display_init();
    if (status != HAL_OK) {
        return status;
    }
    hal_int_state.gpu_display_initialized = true;
    
    status = hal_audio_init();
    if (status != HAL_OK) {
        return status;
    }
    hal_int_state.audio_initialized = true;
    
    status = hal_storage_init();
    if (status != HAL_OK) {
        return status;
    }
    hal_int_state.storage_initialized = true;
    
    status = hal_usb_init();
    if (status != HAL_OK) {
        return status;
    }
    hal_int_state.usb_initialized = true;
    
    status = hal_power_init();
    if (status != HAL_OK) {
        return status;
    }
    hal_int_state.power_initialized = true;
    
    return HAL_OK;
}

hal_status_t hal_fini(void) {
    hal_status_t status = HAL_OK;
    
    if (hal_int_state.power_initialized) {
        status |= hal_power_fini();
    }
    
    if (hal_int_state.usb_initialized) {
        status |= hal_usb_fini();
    }
    
    if (hal_int_state.storage_initialized) {
        status |= hal_storage_fini();
    }
    
    if (hal_int_state.audio_initialized) {
        status |= hal_audio_fini();
    }
    
    if (hal_int_state.gpu_display_initialized) {
        status |= hal_gpu_display_fini();
    }
    
    if (hal_int_state.gpu_initialized) {
    }
    
    if (hal_int_state.acpi_uefi_initialized) {
        status |= hal_acpi_uefi_fini();
    }
    
    if (hal_int_state.firmware_initialized) {
        status |= hal_acpi_fini();
    }
    
    if (hal_int_state.cpu_initialized) {
    }
    
    memset(&hal_int_state, 0, sizeof(hal_internal_state_t));
    
    return (status == 0) ? HAL_OK : HAL_ERR_DEVICE_FAILED;
}

hal_status_t hal_get_system_info(hal_system_info_t *info) {
    if (!info) {
        return HAL_ERR_INVALID_ARG;
    }
    
    info->platform = hal_int_state.platform;
    info->cpu_count = 16;
    info->max_device_count = 256;
    info->uefi_enabled = true;
    info->acpi_enabled = true;
    info->iommu_available = true;
    info->hal_version = 0x00010000;
    
    return HAL_OK;
}

hal_status_t hal_enable_iommu(void) {
    hal_int_state.iommu_enabled = true;
    return HAL_OK;
}

hal_status_t hal_disable_iommu(void) {
    hal_int_state.iommu_enabled = false;
    return HAL_OK;
}

bool hal_is_iommu_enabled(void) {
    return hal_int_state.iommu_enabled;
}

const char *hal_get_error_str(hal_status_t status) {
    switch (status) {
        case HAL_OK:
            return "Success";
        case HAL_ERR_INVALID_ARG:
            return "Invalid argument";
        case HAL_ERR_NOT_SUPPORTED:
            return "Operation not supported";
        case HAL_ERR_DEVICE_BUSY:
            return "Device busy";
        case HAL_ERR_NO_MEMORY:
            return "Out of memory";
        case HAL_ERR_TIMEOUT:
            return "Operation timeout";
        case HAL_ERR_DEVICE_FAILED:
            return "Device failed";
        case HAL_ERR_PERMISSION_DENIED:
            return "Permission denied";
        default:
            return "Unknown error";
    }
}
