#ifndef AEGIS_HAL_H
#define AEGIS_HAL_H

#include <stdint.h>
#include <stdbool.h>

typedef int hal_status_t;

#define HAL_OK 0
#define HAL_ERR_INVALID_ARG -1
#define HAL_ERR_NOT_SUPPORTED -2
#define HAL_ERR_DEVICE_BUSY -3
#define HAL_ERR_NO_MEMORY -4
#define HAL_ERR_TIMEOUT -5
#define HAL_ERR_DEVICE_FAILED -6
#define HAL_ERR_PERMISSION_DENIED -7

typedef enum {
    HAL_PLATFORM_UNKNOWN = 0,
    HAL_PLATFORM_X86_64,
    HAL_PLATFORM_X86_32,
    HAL_PLATFORM_ARM64,
    HAL_PLATFORM_ARM32,
} hal_platform_t;

typedef struct {
    hal_platform_t platform;
    uint8_t cpu_count;
    uint16_t max_device_count;
    bool uefi_enabled;
    bool acpi_enabled;
    bool iommu_available;
    uint32_t hal_version;
} hal_system_info_t;

hal_status_t hal_init(void);
hal_status_t hal_fini(void);
hal_status_t hal_get_system_info(hal_system_info_t *info);
hal_status_t hal_enable_iommu(void);
hal_status_t hal_disable_iommu(void);
bool hal_is_iommu_enabled(void);
const char *hal_get_error_str(hal_status_t status);

#endif
