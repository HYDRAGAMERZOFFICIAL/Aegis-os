#ifndef AEGIS_SERVICES_CHECKPOINT_SYSTEM_H
#define AEGIS_SERVICES_CHECKPOINT_SYSTEM_H

#include <kernel/types.h>

typedef enum {
    CHECKPOINT_TYPE_FULL = 1,
    CHECKPOINT_TYPE_DELTA = 2,
    CHECKPOINT_TYPE_SYSTEM = 3,
    CHECKPOINT_TYPE_USER = 4,
} checkpoint_type_t;

typedef struct {
    char checkpoint_id[64];
    char name[128];
    checkpoint_type_t type;
    uint64_t created_time;
    uint64_t size_bytes;
    uint8_t can_boot;
    char os_version[32];
} checkpoint_info_t;

int checkpoint_init(void);
int checkpoint_create_full_snapshot(const char *name, checkpoint_info_t *info);
int checkpoint_create_delta_snapshot(const char *name, checkpoint_info_t *info);

int checkpoint_list_snapshots(checkpoint_info_t *checkpoints, uint32_t *count, uint32_t max_checkpoints);
int checkpoint_get_info(const char *checkpoint_id, checkpoint_info_t *info);

int checkpoint_rollback(const char *checkpoint_id);
int checkpoint_rollback_with_timeout(const char *checkpoint_id, uint32_t timeout_seconds);

int checkpoint_verify_integrity(const char *checkpoint_id, uint8_t *is_valid);
int checkpoint_repair_checkpoint(const char *checkpoint_id);

int checkpoint_delete(const char *checkpoint_id);
int checkpoint_clone(const char *checkpoint_id, const char *new_name, checkpoint_info_t *new_info);

int checkpoint_export(const char *checkpoint_id, const char *output_file);
int checkpoint_import(const char *input_file, checkpoint_info_t *info);

int checkpoint_create_scheduled_snapshots(uint32_t interval_hours);
int checkpoint_disable_scheduled_snapshots(void);

int checkpoint_estimate_rollback_time(const char *checkpoint_id, uint32_t *estimated_seconds);

#endif
