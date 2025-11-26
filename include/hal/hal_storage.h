#ifndef AEGIS_HAL_STORAGE_H
#define AEGIS_HAL_STORAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "hal.h"

typedef enum {
    HAL_STORAGE_TYPE_UNKNOWN = 0,
    HAL_STORAGE_TYPE_SATA,
    HAL_STORAGE_TYPE_NVME,
    HAL_STORAGE_TYPE_EMMC,
    HAL_STORAGE_TYPE_USB_MASS_STORAGE,
    HAL_STORAGE_TYPE_RAMDISK,
} hal_storage_type_t;

typedef enum {
    HAL_IO_SCHEDULER_DEADLINE = 0,
    HAL_IO_SCHEDULER_CFS = 1,
    HAL_IO_SCHEDULER_NOOP = 2,
} hal_io_scheduler_t;

typedef enum {
    HAL_PARTITION_TYPE_MBR = 0,
    HAL_PARTITION_TYPE_GPT = 1,
} hal_partition_type_t;

typedef enum {
    HAL_STORAGE_BUS_UNKNOWN = 0,
    HAL_STORAGE_BUS_ATA,
    HAL_STORAGE_BUS_SATA,
    HAL_STORAGE_BUS_PCIE,
    HAL_STORAGE_BUS_USB,
    HAL_STORAGE_BUS_SDMMC,
} hal_storage_bus_t;

typedef struct {
    uint8_t device_id;
    hal_storage_type_t type;
    hal_storage_bus_t bus;
    uint64_t total_sectors;
    uint32_t sector_size;
    uint64_t capacity_bytes;
    uint8_t *model;
    uint8_t *serial;
    bool is_removable;
    bool is_attached;
    uint32_t transfer_speed_mbps;
} hal_storage_device_t;

typedef struct {
    uint64_t total_capacity;
    uint64_t free_space;
    uint64_t used_space;
    uint8_t health_percent;
    uint32_t read_speed_mbps;
    uint32_t write_speed_mbps;
    uint64_t read_ops;
    uint64_t write_ops;
} hal_storage_stats_t;

typedef struct {
    uint64_t request_id;
    uint32_t offset;
    uint32_t length;
    uint8_t operation;
    bool is_pending;
    bool is_complete;
    int status;
} hal_storage_io_request_t;

typedef struct {
    uint8_t *smart_data;
    uint16_t data_size;
    bool is_healthy;
    uint8_t temperature;
    uint64_t power_on_hours;
    uint64_t sector_read_count;
    uint64_t sector_write_count;
} hal_storage_smart_t;

typedef struct {
    uint32_t port;
    uint32_t lun;
    uint32_t slot;
    uint16_t vendor_id;
    uint16_t device_id;
} hal_storage_address_t;

typedef struct {
    uint32_t volume_id;
    char name[64];
    uint64_t size_sectors;
    uint64_t allocated_sectors;
    bool encrypted;
    uint8_t encryption_key[32];
    uint32_t owner_container_id;
} hal_storage_volume_t;

typedef struct {
    uint32_t partition_id;
    uint32_t device_id;
    uint64_t start_sector;
    uint64_t size_sectors;
    uint8_t partition_type;
    uint32_t flags;
    char label[64];
} hal_storage_partition_t;

typedef struct {
    uint32_t container_id;
    uint32_t max_iops;
    uint32_t max_bandwidth_mbps;
    uint32_t priority;
    bool throttle_enabled;
} hal_storage_container_qos_t;

typedef void (*hal_storage_io_callback_t)(uint64_t request_id, hal_status_t status, 
                                           void *context);

hal_status_t hal_storage_init(void);
hal_status_t hal_storage_fini(void);
hal_status_t hal_storage_enumerate_devices(uint32_t *device_count);
hal_status_t hal_storage_get_device_info(uint8_t device_id, hal_storage_device_t *device);

hal_status_t hal_storage_attach_device(uint8_t device_id);
hal_status_t hal_storage_detach_device(uint8_t device_id);
hal_status_t hal_storage_is_attached(uint8_t device_id, bool *attached);

hal_status_t hal_storage_read_sync(uint8_t device_id, uint64_t sector, uint32_t sector_count, 
                                    void *buffer, uint32_t timeout_ms);
hal_status_t hal_storage_write_sync(uint8_t device_id, uint64_t sector, 
                                     uint32_t sector_count, const void *buffer, 
                                     uint32_t timeout_ms);
hal_status_t hal_storage_read_async(uint8_t device_id, uint64_t sector, uint32_t sector_count, 
                                     void *buffer, hal_storage_io_callback_t callback, 
                                     void *context, uint64_t *request_id);
hal_status_t hal_storage_write_async(uint8_t device_id, uint64_t sector, 
                                      uint32_t sector_count, const void *buffer, 
                                      hal_storage_io_callback_t callback, void *context, 
                                      uint64_t *request_id);

hal_status_t hal_storage_cancel_request(uint64_t request_id);
hal_status_t hal_storage_get_request_status(uint64_t request_id, 
                                             hal_storage_io_request_t *request);

hal_status_t hal_storage_flush_cache(uint8_t device_id);
hal_status_t hal_storage_get_stats(uint8_t device_id, hal_storage_stats_t *stats);
hal_status_t hal_storage_get_smart_info(uint8_t device_id, hal_storage_smart_t *smart);

hal_status_t hal_storage_trim(uint8_t device_id, uint64_t sector, uint32_t sector_count);
hal_status_t hal_storage_format(uint8_t device_id, uint32_t sector_size);
hal_status_t hal_storage_reset_device(uint8_t device_id);

hal_status_t hal_storage_nvme_identify_controller(uint8_t device_id, void *buf, 
                                                   uint32_t buf_size);
hal_status_t hal_storage_nvme_identify_namespace(uint8_t device_id, uint32_t nsid, 
                                                  void *buf, uint32_t buf_size);
hal_status_t hal_storage_nvme_submit_admin_command(uint8_t device_id, const void *cmd, 
                                                    uint32_t cmd_size, void *response, 
                                                    uint32_t resp_size);

hal_status_t hal_storage_sata_get_port_info(uint8_t device_id, uint32_t *port, 
                                             uint32_t *lun);
hal_status_t hal_storage_sata_send_fis(uint8_t device_id, const void *fis, uint32_t fis_size);

hal_status_t hal_storage_is_healthy(uint8_t device_id, bool *healthy);
hal_status_t hal_storage_get_error_count(uint8_t device_id, uint64_t *error_count);

hal_status_t hal_storage_lock_device(uint8_t device_id);
hal_status_t hal_storage_unlock_device(uint8_t device_id, const uint8_t *password, 
                                        uint32_t password_len);
hal_status_t hal_storage_set_password(uint8_t device_id, const uint8_t *password, 
                                       uint32_t password_len);

hal_status_t hal_storage_create_volume(uint8_t device_id, uint64_t size_sectors, 
                                        const char *name, uint32_t *volume_id);
hal_status_t hal_storage_delete_volume(uint32_t volume_id);
hal_status_t hal_storage_enumerate_volumes(hal_storage_volume_t *volumes, uint32_t *volume_count);
hal_status_t hal_storage_get_volume(uint32_t volume_id, hal_storage_volume_t *volume);
hal_status_t hal_storage_mount_volume(uint32_t volume_id, uint32_t container_id);
hal_status_t hal_storage_unmount_volume(uint32_t volume_id);

hal_status_t hal_storage_create_partition(uint8_t device_id, uint64_t start_sector, 
                                          uint64_t size_sectors, const char *label,
                                          uint32_t *partition_id);
hal_status_t hal_storage_delete_partition(uint32_t partition_id);
hal_status_t hal_storage_enumerate_partitions(uint8_t device_id, hal_storage_partition_t *partitions,
                                              uint32_t *partition_count);
hal_status_t hal_storage_get_partition_type(uint8_t device_id, hal_partition_type_t *type);
hal_status_t hal_storage_set_partition_type(uint8_t device_id, hal_partition_type_t type);

hal_status_t hal_storage_set_io_scheduler(uint8_t device_id, hal_io_scheduler_t scheduler);
hal_status_t hal_storage_get_io_scheduler(uint8_t device_id, hal_io_scheduler_t *scheduler);

hal_status_t hal_storage_set_container_qos(const hal_storage_container_qos_t *qos);
hal_status_t hal_storage_get_container_qos(uint32_t container_id, 
                                           hal_storage_container_qos_t *qos);
hal_status_t hal_storage_enable_container_throttle(uint32_t container_id);
hal_status_t hal_storage_disable_container_throttle(uint32_t container_id);

#endif
