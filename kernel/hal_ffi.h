#ifndef AEGIS_HAL_FFI_H
#define AEGIS_HAL_FFI_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    HAL_FFI_OK = 0,
    HAL_FFI_INVALID_ARG = -1,
    HAL_FFI_NOT_INITIALIZED = -2,
    HAL_FFI_OPERATION_FAILED = -3,
    HAL_FFI_TIMEOUT = -4,
    HAL_FFI_NOT_SUPPORTED = -5,
    HAL_FFI_BUFFER_OVERFLOW = -6,
    HAL_FFI_MEMORY_ERROR = -7
} hal_ffi_error_t;

typedef int32_t hal_ffi_handle_t;
typedef void (*hal_ffi_callback_t)(hal_ffi_handle_t handle, void *context);
typedef void (*hal_ffi_error_callback_t)(hal_ffi_error_t error, const char *message);

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} hal_ffi_color_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t refresh_rate;
    uint8_t bits_per_pixel;
} hal_ffi_resolution_t;

typedef struct {
    uint64_t frequency_mhz;
    uint32_t cores;
    uint32_t threads;
    uint64_t cache_size;
    bool supports_virtualization;
    bool supports_nested_paging;
} hal_ffi_cpu_info_t;

typedef struct {
    uint64_t total_memory;
    uint64_t available_memory;
    uint32_t memory_type;
    bool ecc_enabled;
} hal_ffi_memory_info_t;

typedef struct {
    char vendor[256];
    char model[256];
    uint64_t capacity;
    uint32_t block_size;
    uint8_t type;
} hal_ffi_storage_info_t;

typedef struct {
    uint8_t *buffer;
    size_t size;
    size_t offset;
} hal_ffi_buffer_t;

typedef struct {
    hal_ffi_error_t error_code;
    char error_message[512];
    hal_ffi_handle_t failed_handle;
    uint64_t timestamp;
} hal_ffi_error_info_t;

hal_ffi_error_t hal_ffi_init(void);
hal_ffi_error_t hal_ffi_shutdown(void);
hal_ffi_error_t hal_ffi_get_last_error(hal_ffi_error_info_t *error);
void hal_ffi_register_error_callback(hal_ffi_error_callback_t callback);

hal_ffi_error_t hal_ffi_cpu_get_info(hal_ffi_handle_t cpu_id, hal_ffi_cpu_info_t *info);
hal_ffi_error_t hal_ffi_cpu_set_frequency(hal_ffi_handle_t cpu_id, uint64_t frequency_mhz);
hal_ffi_error_t hal_ffi_cpu_get_frequency(hal_ffi_handle_t cpu_id, uint64_t *frequency_mhz);
hal_ffi_error_t hal_ffi_cpu_enable_virtualization(hal_ffi_handle_t cpu_id);
hal_ffi_error_t hal_ffi_cpu_disable_virtualization(hal_ffi_handle_t cpu_id);

hal_ffi_error_t hal_ffi_memory_get_info(hal_ffi_memory_info_t *info);
hal_ffi_error_t hal_ffi_memory_allocate(size_t size, hal_ffi_buffer_t *buffer);
hal_ffi_error_t hal_ffi_memory_free(hal_ffi_buffer_t *buffer);
hal_ffi_error_t hal_ffi_memory_map_dma(uint64_t phys_addr, size_t size, void **virt_addr);
hal_ffi_error_t hal_ffi_memory_unmap_dma(void *virt_addr);

hal_ffi_error_t hal_ffi_storage_get_info(hal_ffi_handle_t dev_id, hal_ffi_storage_info_t *info);
hal_ffi_error_t hal_ffi_storage_read(hal_ffi_handle_t dev_id, uint64_t lba, size_t count, 
                                      void *buffer);
hal_ffi_error_t hal_ffi_storage_write(hal_ffi_handle_t dev_id, uint64_t lba, size_t count,
                                       const void *buffer);
hal_ffi_error_t hal_ffi_storage_flush(hal_ffi_handle_t dev_id);

hal_ffi_error_t hal_ffi_gpu_get_resolution(hal_ffi_resolution_t *resolution);
hal_ffi_error_t hal_ffi_gpu_set_resolution(const hal_ffi_resolution_t *resolution);
hal_ffi_error_t hal_ffi_gpu_set_framebuffer(uint64_t physical_addr, size_t size);
hal_ffi_error_t hal_ffi_gpu_draw_pixel(uint32_t x, uint32_t y, hal_ffi_color_t color);
hal_ffi_error_t hal_ffi_gpu_fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                                       hal_ffi_color_t color);

hal_ffi_error_t hal_ffi_power_get_battery_level(uint32_t *level_percent);
hal_ffi_error_t hal_ffi_power_get_battery_status(uint8_t *status);
hal_ffi_error_t hal_ffi_power_set_sleep_mode(uint8_t mode);
hal_ffi_error_t hal_ffi_power_reboot(void);
hal_ffi_error_t hal_ffi_power_shutdown(void);

hal_ffi_error_t hal_ffi_interrupt_register_handler(uint32_t irq, hal_ffi_callback_t handler,
                                                    void *context);
hal_ffi_error_t hal_ffi_interrupt_unregister_handler(uint32_t irq);
hal_ffi_error_t hal_ffi_interrupt_enable(uint32_t irq);
hal_ffi_error_t hal_ffi_interrupt_disable(uint32_t irq);

hal_ffi_error_t hal_ffi_acpi_get_device_info(const char *device_id, void *info);
hal_ffi_error_t hal_ffi_acpi_evaluate_object(const char *path, void *args, void *result);
hal_ffi_error_t hal_ffi_uefi_get_variable(const char *name, void *value, size_t *size);
hal_ffi_error_t hal_ffi_uefi_set_variable(const char *name, const void *value, size_t size);

hal_ffi_error_t hal_ffi_string_to_error(const char *error_str);
const char *hal_ffi_error_to_string(hal_ffi_error_t error);
hal_ffi_error_t hal_ffi_type_convert_u32_to_u64(uint32_t input, uint64_t *output);
hal_ffi_error_t hal_ffi_type_convert_u64_to_u32(uint64_t input, uint32_t *output);

#endif
