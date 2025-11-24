#ifndef AEGIS_DRIVERS_STORAGE_H
#define AEGIS_DRIVERS_STORAGE_H

#include <kernel/types.h>

typedef enum {
    STORAGE_TYPE_NVME = 1,
    STORAGE_TYPE_SATA = 2,
    STORAGE_TYPE_USB = 3,
    STORAGE_TYPE_SD_CARD = 4,
    STORAGE_TYPE_EMMC = 5,
} storage_device_type_t;

typedef enum {
    STORAGE_STATUS_OK = 0,
    STORAGE_STATUS_ERROR = 1,
    STORAGE_STATUS_BUSY = 2,
    STORAGE_STATUS_NOT_READY = 3,
} storage_status_t;

typedef struct {
    char name[64];
    storage_device_type_t type;
    uint64_t capacity_bytes;
    uint32_t sector_size;
    uint32_t block_size;
    uint8_t removable;
    uint8_t encrypted;
} storage_device_info_t;

typedef struct {
    uint64_t sector;
    uint32_t count;
    uint8_t *buffer;
    uint32_t timeout_ms;
} storage_io_request_t;

typedef struct storage_device storage_device_t;

typedef void (*storage_completion_callback_fn)(storage_device_t *dev, int status, void *ctx);

int storage_driver_init(void);
int storage_driver_shutdown(void);

int storage_enumerate_devices(storage_device_info_t *devices, uint32_t *count, uint32_t max_devices);
storage_device_t *storage_open_device(uint32_t device_id);
int storage_close_device(storage_device_t *dev);

int storage_read_sectors(storage_device_t *dev, uint64_t sector, uint32_t count, void *buffer);
int storage_write_sectors(storage_device_t *dev, uint64_t sector, uint32_t count, const void *buffer);

int storage_read_sectors_async(storage_device_t *dev, storage_io_request_t *req, 
                               storage_completion_callback_fn callback, void *ctx);
int storage_write_sectors_async(storage_device_t *dev, storage_io_request_t *req,
                                storage_completion_callback_fn callback, void *ctx);

int storage_get_device_info(storage_device_t *dev, storage_device_info_t *info);
storage_status_t storage_get_status(storage_device_t *dev);

int storage_format_device(storage_device_t *dev, const char *filesystem_type);
int storage_check_filesystem(storage_device_t *dev, uint8_t repair);

int storage_enable_encryption(storage_device_t *dev, const uint8_t *key, uint32_t key_len);
int storage_disable_encryption(storage_device_t *dev);
int storage_is_encrypted(storage_device_t *dev);

int storage_enable_auto_encrypt_external(void);
int storage_disable_auto_encrypt_external(void);
int storage_get_auto_encrypt_status(void);

int storage_trim_device(storage_device_t *dev);
int storage_flush_cache(storage_device_t *dev);

int storage_set_power_mode(storage_device_t *dev, uint8_t power_state);
int storage_get_power_mode(storage_device_t *dev);

uint32_t storage_get_queue_depth(storage_device_t *dev);
int storage_cancel_io(storage_device_t *dev, uint64_t io_id);

int storage_smart_get_health(storage_device_t *dev, char *health_data, uint32_t max_len);
int storage_smart_enable_monitoring(storage_device_t *dev);

#endif
