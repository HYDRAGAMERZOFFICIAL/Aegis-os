#ifndef AEGIS_STORAGE_MANAGER_H
#define AEGIS_STORAGE_MANAGER_H

#include <kernel/types.h>

typedef enum {
    DISK_TYPE_SSD,
    DISK_TYPE_HDD,
    DISK_TYPE_NVME,
    DISK_TYPE_RAMDISK
} disk_type_t;

typedef enum {
    VOLUME_STATE_UNMOUNTED,
    VOLUME_STATE_MOUNTING,
    VOLUME_STATE_MOUNTED,
    VOLUME_STATE_READONLY,
    VOLUME_STATE_FAILED,
    VOLUME_STATE_ENCRYPTING
} volume_state_t;

typedef struct {
    u64 disk_id;
    disk_type_t type;
    u64 total_size;
    u64 used_size;
    u32 health_percent;
} disk_t;

typedef struct {
    u64 volume_id;
    const char *mount_point;
    volume_state_t state;
    bool encrypted;
    u8 encryption_key[32];
    u64 total_size;
    u64 used_size;
} volume_t;

int storage_mgr_init(void);
disk_t **storage_mgr_get_disks(u32 *count);
volume_t **storage_mgr_get_volumes(u32 *count);
int storage_mgr_mount_volume(u64 volume_id, const char *mount_point);
int storage_mgr_unmount_volume(u64 volume_id);
int storage_mgr_create_partition(u64 disk_id, u64 size, const char *label);
int storage_mgr_delete_partition(u64 volume_id);
int storage_mgr_enable_encryption(u64 volume_id, const u8 *key);
int storage_mgr_disable_encryption(u64 volume_id);
int storage_mgr_verify_integrity(u64 volume_id);
int storage_mgr_repair_volume(u64 volume_id);
int storage_mgr_get_disk_health(u64 disk_id, u32 *health);
volume_state_t storage_mgr_get_volume_state(u64 volume_id);

#endif
