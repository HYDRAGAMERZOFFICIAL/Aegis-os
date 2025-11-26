#include <string.h>
#include <stdlib.h>
#include "../include/hal/hal_storage.h"

#define MAX_STORAGE_DEVICES 16
#define MAX_IO_REQUESTS 256
#define MAX_VOLUMES 32
#define MAX_PARTITIONS 256
#define MAX_CONTAINER_QOS 32

typedef struct {
    uint8_t device_count;
    hal_storage_device_t devices[MAX_STORAGE_DEVICES];
    uint64_t request_counter;
    uint32_t volume_counter;
    uint32_t partition_counter;
    uint32_t volume_count;
    uint32_t partition_count;
    uint32_t qos_count;
    hal_storage_volume_t volumes[MAX_VOLUMES];
    hal_storage_partition_t partitions[MAX_PARTITIONS];
    hal_storage_container_qos_t qos_list[MAX_CONTAINER_QOS];
    hal_io_scheduler_t scheduler[MAX_STORAGE_DEVICES];
    bool initialized;
} storage_hal_state_t;

static storage_hal_state_t storage_state = {0};

hal_status_t hal_storage_init(void) {
    if (storage_state.initialized) {
        return HAL_OK;
    }
    
    memset(&storage_state, 0, sizeof(storage_hal_state_t));
    
    storage_state.device_count = 2;
    
    storage_state.devices[0].device_id = 0;
    storage_state.devices[0].type = HAL_STORAGE_TYPE_NVME;
    storage_state.devices[0].bus = HAL_STORAGE_BUS_PCIE;
    storage_state.devices[0].total_sectors = 1000 * 1024 * 1024;
    storage_state.devices[0].sector_size = 512;
    storage_state.devices[0].capacity_bytes = 512ULL * 1000 * 1024 * 1024;
    storage_state.devices[0].model = (uint8_t *)"Samsung 980 Pro";
    storage_state.devices[0].serial = (uint8_t *)"SN1234567890";
    storage_state.devices[0].is_removable = false;
    storage_state.devices[0].is_attached = true;
    storage_state.devices[0].transfer_speed_mbps = 7000;
    
    storage_state.devices[1].device_id = 1;
    storage_state.devices[1].type = HAL_STORAGE_TYPE_SATA;
    storage_state.devices[1].bus = HAL_STORAGE_BUS_SATA;
    storage_state.devices[1].total_sectors = 2000 * 1024 * 1024;
    storage_state.devices[1].sector_size = 512;
    storage_state.devices[1].capacity_bytes = 512ULL * 2000 * 1024 * 1024;
    storage_state.devices[1].model = (uint8_t *)"WDC Red Pro";
    storage_state.devices[1].serial = (uint8_t *)"WD0987654321";
    storage_state.devices[1].is_removable = false;
    storage_state.devices[1].is_attached = true;
    storage_state.devices[1].transfer_speed_mbps = 550;
    
    storage_state.request_counter = 2000;
    storage_state.initialized = true;
    
    return HAL_OK;
}

hal_status_t hal_storage_fini(void) {
    storage_state.initialized = false;
    return HAL_OK;
}

hal_status_t hal_storage_enumerate_devices(uint32_t *device_count) {
    if (!device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    *device_count = storage_state.device_count;
    return HAL_OK;
}

hal_status_t hal_storage_get_device_info(uint8_t device_id, hal_storage_device_t *device) {
    if (!device || device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    *device = storage_state.devices[device_id];
    return HAL_OK;
}

hal_status_t hal_storage_attach_device(uint8_t device_id) {
    if (device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    storage_state.devices[device_id].is_attached = true;
    return HAL_OK;
}

hal_status_t hal_storage_detach_device(uint8_t device_id) {
    if (device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    storage_state.devices[device_id].is_attached = false;
    return HAL_OK;
}

hal_status_t hal_storage_is_attached(uint8_t device_id, bool *attached) {
    if (!attached || device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    *attached = storage_state.devices[device_id].is_attached;
    return HAL_OK;
}

hal_status_t hal_storage_read_sync(uint8_t device_id, uint64_t sector, uint32_t sector_count, 
                                    void *buffer, uint32_t timeout_ms) {
    if (!buffer || device_id >= storage_state.device_count || sector_count == 0) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.devices[device_id].is_attached) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    memset(buffer, 0xAA, sector_count * storage_state.devices[device_id].sector_size);
    return HAL_OK;
}

hal_status_t hal_storage_write_sync(uint8_t device_id, uint64_t sector, 
                                     uint32_t sector_count, const void *buffer, 
                                     uint32_t timeout_ms) {
    if (!buffer || device_id >= storage_state.device_count || sector_count == 0) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.devices[device_id].is_attached) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    return HAL_OK;
}

hal_status_t hal_storage_read_async(uint8_t device_id, uint64_t sector, uint32_t sector_count, 
                                     void *buffer, hal_storage_io_callback_t callback, 
                                     void *context, uint64_t *request_id) {
    if (!buffer || !callback || !request_id || device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    *request_id = storage_state.request_counter++;
    return HAL_OK;
}

hal_status_t hal_storage_write_async(uint8_t device_id, uint64_t sector, 
                                      uint32_t sector_count, const void *buffer, 
                                      hal_storage_io_callback_t callback, void *context, 
                                      uint64_t *request_id) {
    if (!buffer || !callback || !request_id || device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    *request_id = storage_state.request_counter++;
    return HAL_OK;
}

hal_status_t hal_storage_cancel_request(uint64_t request_id) {
    if (request_id == 0) {
        return HAL_ERR_INVALID_ARG;
    }
    
    return HAL_OK;
}

hal_status_t hal_storage_get_request_status(uint64_t request_id, 
                                             hal_storage_io_request_t *request) {
    if (!request || request_id == 0) {
        return HAL_ERR_INVALID_ARG;
    }
    
    request->request_id = request_id;
    request->is_complete = true;
    request->status = HAL_OK;
    
    return HAL_OK;
}

hal_status_t hal_storage_flush_cache(uint8_t device_id) {
    if (device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    return HAL_OK;
}

hal_status_t hal_storage_get_stats(uint8_t device_id, hal_storage_stats_t *stats) {
    if (!stats || device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    stats->total_capacity = storage_state.devices[device_id].capacity_bytes;
    stats->free_space = stats->total_capacity / 2;
    stats->used_space = stats->total_capacity / 2;
    stats->health_percent = 95;
    stats->read_speed_mbps = storage_state.devices[device_id].transfer_speed_mbps;
    stats->write_speed_mbps = storage_state.devices[device_id].transfer_speed_mbps;
    stats->read_ops = 1000000;
    stats->write_ops = 500000;
    
    return HAL_OK;
}

hal_status_t hal_storage_get_smart_info(uint8_t device_id, hal_storage_smart_t *smart) {
    if (!smart || device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    smart->is_healthy = true;
    smart->temperature = 45;
    smart->power_on_hours = 5000;
    smart->sector_read_count = 10000000;
    smart->sector_write_count = 5000000;
    
    return HAL_OK;
}

hal_status_t hal_storage_trim(uint8_t device_id, uint64_t sector, uint32_t sector_count) {
    if (device_id >= storage_state.device_count || sector_count == 0) {
        return HAL_ERR_INVALID_ARG;
    }
    
    return HAL_OK;
}

hal_status_t hal_storage_format(uint8_t device_id, uint32_t sector_size) {
    if (device_id >= storage_state.device_count || sector_size == 0) {
        return HAL_ERR_INVALID_ARG;
    }
    
    return HAL_OK;
}

hal_status_t hal_storage_reset_device(uint8_t device_id) {
    if (device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    return HAL_OK;
}

hal_status_t hal_storage_nvme_identify_controller(uint8_t device_id, void *buf, 
                                                   uint32_t buf_size) {
    if (!buf || device_id >= storage_state.device_count || buf_size == 0) {
        return HAL_ERR_INVALID_ARG;
    }
    
    memset(buf, 0, buf_size);
    return HAL_OK;
}

hal_status_t hal_storage_nvme_identify_namespace(uint8_t device_id, uint32_t nsid, 
                                                  void *buf, uint32_t buf_size) {
    if (!buf || device_id >= storage_state.device_count || buf_size == 0) {
        return HAL_ERR_INVALID_ARG;
    }
    
    memset(buf, 0, buf_size);
    return HAL_OK;
}

hal_status_t hal_storage_nvme_submit_admin_command(uint8_t device_id, const void *cmd, 
                                                    uint32_t cmd_size, void *response, 
                                                    uint32_t resp_size) {
    if (!cmd || !response || device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    memset(response, 0, resp_size);
    return HAL_OK;
}

hal_status_t hal_storage_sata_get_port_info(uint8_t device_id, uint32_t *port, 
                                             uint32_t *lun) {
    if (!port || !lun || device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    *port = device_id;
    *lun = 0;
    
    return HAL_OK;
}

hal_status_t hal_storage_sata_send_fis(uint8_t device_id, const void *fis, uint32_t fis_size) {
    if (!fis || device_id >= storage_state.device_count || fis_size == 0) {
        return HAL_ERR_INVALID_ARG;
    }
    
    return HAL_OK;
}

hal_status_t hal_storage_is_healthy(uint8_t device_id, bool *healthy) {
    if (!healthy || device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    *healthy = true;
    return HAL_OK;
}

hal_status_t hal_storage_get_error_count(uint8_t device_id, uint64_t *error_count) {
    if (!error_count || device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    *error_count = 0;
    return HAL_OK;
}

hal_status_t hal_storage_lock_device(uint8_t device_id) {
    if (device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    return HAL_OK;
}

hal_status_t hal_storage_unlock_device(uint8_t device_id, const uint8_t *password, 
                                        uint32_t password_len) {
    if (!password || password_len == 0 || device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    return HAL_OK;
}

hal_status_t hal_storage_set_password(uint8_t device_id, const uint8_t *password, 
                                       uint32_t password_len) {
    if (!password || password_len == 0 || device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    return HAL_OK;
}

hal_status_t hal_storage_create_volume(uint8_t device_id, uint64_t size_sectors, 
                                        const char *name, uint32_t *volume_id) {
    if (!name || !volume_id || device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    if (storage_state.volume_count >= MAX_VOLUMES) {
        return HAL_ERR_NO_MEMORY;
    }
    
    hal_storage_volume_t *vol = &storage_state.volumes[storage_state.volume_count];
    vol->volume_id = ++storage_state.volume_counter;
    vol->size_sectors = size_sectors;
    vol->allocated_sectors = 0;
    vol->encrypted = false;
    vol->owner_container_id = 0;
    strncpy(vol->name, name, 63);
    
    *volume_id = vol->volume_id;
    storage_state.volume_count++;
    
    return HAL_OK;
}

hal_status_t hal_storage_delete_volume(uint32_t volume_id) {
    if (volume_id == 0) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < storage_state.volume_count; i++) {
        if (storage_state.volumes[i].volume_id == volume_id) {
            if (i < storage_state.volume_count - 1) {
                memmove(&storage_state.volumes[i], &storage_state.volumes[i + 1],
                        (storage_state.volume_count - i - 1) * sizeof(hal_storage_volume_t));
            }
            storage_state.volume_count--;
            return HAL_OK;
        }
    }
    
    return HAL_ERR_DEVICE_FAILED;
}

hal_status_t hal_storage_enumerate_volumes(hal_storage_volume_t *volumes, uint32_t *volume_count) {
    if (!volumes || !volume_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    if (*volume_count < storage_state.volume_count) {
        *volume_count = storage_state.volume_count;
        return HAL_ERR_INVALID_ARG;
    }
    
    memcpy(volumes, storage_state.volumes, storage_state.volume_count * sizeof(hal_storage_volume_t));
    *volume_count = storage_state.volume_count;
    
    return HAL_OK;
}

hal_status_t hal_storage_get_volume(uint32_t volume_id, hal_storage_volume_t *volume) {
    if (!volume || volume_id == 0) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < storage_state.volume_count; i++) {
        if (storage_state.volumes[i].volume_id == volume_id) {
            memcpy(volume, &storage_state.volumes[i], sizeof(hal_storage_volume_t));
            return HAL_OK;
        }
    }
    
    return HAL_ERR_DEVICE_FAILED;
}

hal_status_t hal_storage_mount_volume(uint32_t volume_id, uint32_t container_id) {
    if (volume_id == 0) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < storage_state.volume_count; i++) {
        if (storage_state.volumes[i].volume_id == volume_id) {
            storage_state.volumes[i].owner_container_id = container_id;
            return HAL_OK;
        }
    }
    
    return HAL_ERR_DEVICE_FAILED;
}

hal_status_t hal_storage_unmount_volume(uint32_t volume_id) {
    if (volume_id == 0) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < storage_state.volume_count; i++) {
        if (storage_state.volumes[i].volume_id == volume_id) {
            storage_state.volumes[i].owner_container_id = 0;
            return HAL_OK;
        }
    }
    
    return HAL_ERR_DEVICE_FAILED;
}

hal_status_t hal_storage_create_partition(uint8_t device_id, uint64_t start_sector, 
                                          uint64_t size_sectors, const char *label,
                                          uint32_t *partition_id) {
    if (!label || !partition_id || device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    if (storage_state.partition_count >= MAX_PARTITIONS) {
        return HAL_ERR_NO_MEMORY;
    }
    
    hal_storage_partition_t *part = &storage_state.partitions[storage_state.partition_count];
    part->partition_id = ++storage_state.partition_counter;
    part->device_id = device_id;
    part->start_sector = start_sector;
    part->size_sectors = size_sectors;
    part->partition_type = HAL_PARTITION_TYPE_GPT;
    part->flags = 0;
    strncpy(part->label, label, 63);
    
    *partition_id = part->partition_id;
    storage_state.partition_count++;
    
    return HAL_OK;
}

hal_status_t hal_storage_delete_partition(uint32_t partition_id) {
    if (partition_id == 0) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < storage_state.partition_count; i++) {
        if (storage_state.partitions[i].partition_id == partition_id) {
            if (i < storage_state.partition_count - 1) {
                memmove(&storage_state.partitions[i], &storage_state.partitions[i + 1],
                        (storage_state.partition_count - i - 1) * sizeof(hal_storage_partition_t));
            }
            storage_state.partition_count--;
            return HAL_OK;
        }
    }
    
    return HAL_ERR_DEVICE_FAILED;
}

hal_status_t hal_storage_enumerate_partitions(uint8_t device_id, hal_storage_partition_t *partitions,
                                              uint32_t *partition_count) {
    if (!partitions || !partition_count || device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    uint32_t count = 0;
    for (uint32_t i = 0; i < storage_state.partition_count; i++) {
        if (storage_state.partitions[i].device_id == device_id) {
            count++;
        }
    }
    
    if (*partition_count < count) {
        *partition_count = count;
        return HAL_ERR_INVALID_ARG;
    }
    
    uint32_t idx = 0;
    for (uint32_t i = 0; i < storage_state.partition_count; i++) {
        if (storage_state.partitions[i].device_id == device_id) {
            memcpy(&partitions[idx], &storage_state.partitions[i], sizeof(hal_storage_partition_t));
            idx++;
        }
    }
    *partition_count = count;
    
    return HAL_OK;
}

hal_status_t hal_storage_get_partition_type(uint8_t device_id, hal_partition_type_t *type) {
    if (!type || device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    *type = HAL_PARTITION_TYPE_GPT;
    
    return HAL_OK;
}

hal_status_t hal_storage_set_partition_type(uint8_t device_id, hal_partition_type_t type) {
    if (device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    return HAL_OK;
}

hal_status_t hal_storage_set_io_scheduler(uint8_t device_id, hal_io_scheduler_t scheduler) {
    if (device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    storage_state.scheduler[device_id] = scheduler;
    
    return HAL_OK;
}

hal_status_t hal_storage_get_io_scheduler(uint8_t device_id, hal_io_scheduler_t *scheduler) {
    if (!scheduler || device_id >= storage_state.device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    *scheduler = storage_state.scheduler[device_id];
    
    return HAL_OK;
}

hal_status_t hal_storage_set_container_qos(const hal_storage_container_qos_t *qos) {
    if (!qos) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < storage_state.qos_count; i++) {
        if (storage_state.qos_list[i].container_id == qos->container_id) {
            memcpy(&storage_state.qos_list[i], qos, sizeof(hal_storage_container_qos_t));
            return HAL_OK;
        }
    }
    
    if (storage_state.qos_count < MAX_CONTAINER_QOS) {
        memcpy(&storage_state.qos_list[storage_state.qos_count], qos, sizeof(hal_storage_container_qos_t));
        storage_state.qos_count++;
        return HAL_OK;
    }
    
    return HAL_ERR_NO_MEMORY;
}

hal_status_t hal_storage_get_container_qos(uint32_t container_id, 
                                           hal_storage_container_qos_t *qos) {
    if (!qos) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < storage_state.qos_count; i++) {
        if (storage_state.qos_list[i].container_id == container_id) {
            memcpy(qos, &storage_state.qos_list[i], sizeof(hal_storage_container_qos_t));
            return HAL_OK;
        }
    }
    
    return HAL_ERR_DEVICE_FAILED;
}

hal_status_t hal_storage_enable_container_throttle(uint32_t container_id) {
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < storage_state.qos_count; i++) {
        if (storage_state.qos_list[i].container_id == container_id) {
            storage_state.qos_list[i].throttle_enabled = true;
            return HAL_OK;
        }
    }
    
    return HAL_ERR_DEVICE_FAILED;
}

hal_status_t hal_storage_disable_container_throttle(uint32_t container_id) {
    if (!storage_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < storage_state.qos_count; i++) {
        if (storage_state.qos_list[i].container_id == container_id) {
            storage_state.qos_list[i].throttle_enabled = false;
            return HAL_OK;
        }
    }
    
    return HAL_ERR_DEVICE_FAILED;
}
