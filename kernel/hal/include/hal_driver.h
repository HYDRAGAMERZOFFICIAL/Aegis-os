#ifndef __KERNEL_HAL_DRIVER_H__
#define __KERNEL_HAL_DRIVER_H__

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

typedef uint32_t hal_device_id_t;
typedef uint32_t hal_driver_id_t;
typedef uint32_t hal_capability_token_t;

typedef enum {
    HAL_DEVICE_CLASS_CPU = 0x01,
    HAL_DEVICE_CLASS_GPU = 0x02,
    HAL_DEVICE_CLASS_AUDIO = 0x03,
    HAL_DEVICE_CLASS_BLOCK = 0x04,
    HAL_DEVICE_CLASS_USB = 0x05,
    HAL_DEVICE_CLASS_NETWORK = 0x06,
    HAL_DEVICE_CLASS_SENSOR = 0x07,
    HAL_DEVICE_CLASS_ACPI = 0x08,
    HAL_DEVICE_CLASS_UEFI = 0x09,
} hal_device_class_t;

typedef enum {
    HAL_DRIVER_INIT = 0,
    HAL_DRIVER_PROBE = 1,
    HAL_DRIVER_SUSPEND = 2,
    HAL_DRIVER_RESUME = 3,
    HAL_DRIVER_REMOVE = 4,
    HAL_DRIVER_SHUTDOWN = 5,
} hal_driver_callback_t;

typedef struct {
    hal_driver_id_t driver_id;
    const char *driver_name;
    uint32_t version;
    uint32_t flags;
    
    void *priv;
} hal_driver_t;

typedef struct {
    hal_device_id_t device_id;
    hal_device_class_t device_class;
    const char *device_name;
    uint32_t flags;
    
    void *priv;
} hal_device_t;

typedef struct {
    hal_capability_token_t token;
    uint32_t container_id;
    uint32_t app_id;
    uint32_t permissions;
    uint64_t expiry_time;
} hal_capability_t;

typedef hal_status_t (*hal_callback_fn_t)(
    hal_device_t *device,
    hal_driver_callback_t callback_type,
    void *callback_data
);

typedef struct {
    hal_driver_id_t driver_id;
    const char *driver_name;
    uint32_t version;
    
    hal_callback_fn_t init;
    hal_callback_fn_t probe;
    hal_callback_fn_t suspend;
    hal_callback_fn_t resume;
    hal_callback_fn_t remove;
    
    uint32_t priority;
} hal_driver_descriptor_t;

hal_status_t hal_driver_register(const hal_driver_descriptor_t *descriptor);
hal_status_t hal_driver_unregister(hal_driver_id_t driver_id);
hal_status_t hal_device_probe(hal_device_t *device, hal_driver_t *driver);
hal_status_t hal_capability_request(uint32_t container_id, uint32_t permissions,
                                     hal_capability_t *cap);
hal_status_t hal_capability_verify(const hal_capability_t *cap);

#endif
