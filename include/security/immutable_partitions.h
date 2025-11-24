#ifndef AEGIS_SECURITY_IMMUTABLE_H
#define AEGIS_SECURITY_IMMUTABLE_H

#include <kernel/types.h>

typedef enum {
    IMMUT_PARTITION_ROOT = 1,
    IMMUT_PARTITION_SYSTEM = 2,
    IMMUT_PARTITION_KERNEL = 3,
    IMMUT_PARTITION_BOOT = 4,
} immut_partition_type_t;

typedef struct {
    char name[64];
    immut_partition_type_t type;
    uint64_t start_block;
    uint64_t size_bytes;
    uint8_t hash[64];
    uint8_t is_locked;
} immut_partition_info_t;

int immut_init(void);
int immut_lock_partition(immut_partition_type_t partition);
int immut_unlock_partition(immut_partition_type_t partition);
int immut_get_partition_info(immut_partition_type_t partition, immut_partition_info_t *info);
int immut_verify_partition(immut_partition_type_t partition, uint8_t *expected_hash);
int immut_create_snapshot(immut_partition_type_t partition, const char *snapshot_name);
int immut_rollback_snapshot(immut_partition_type_t partition, const char *snapshot_name);
int immut_detect_tampering(immut_partition_type_t partition);
int immut_auto_repair(immut_partition_type_t partition);

#endif
