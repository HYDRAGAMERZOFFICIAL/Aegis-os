#ifndef AEGIS_SNAPSHOT_VERSIONING_H
#define AEGIS_SNAPSHOT_VERSIONING_H

#include <kernel/types.h>

typedef enum {
    SNAPSHOT_TYPE_SYSTEM,
    SNAPSHOT_TYPE_USER,
    SNAPSHOT_TYPE_BACKUP
} snapshot_type_t;

typedef struct {
    u64 snapshot_id;
    const char *snapshot_name;
    snapshot_type_t type;
    u64 creation_time;
    u64 size;
    const char *description;
    bool is_incremental;
} snapshot_t;

typedef struct {
    u64 version_id;
    const char *file_path;
    u64 version_number;
    u64 timestamp;
    u32 author_uid;
    const char *change_description;
    u8 file_hash[32];
} file_version_t;

int snapshot_init(void);
snapshot_t *snapshot_create(const char *name, snapshot_type_t type);
int snapshot_restore(u64 snapshot_id);
int snapshot_delete(u64 snapshot_id);
int snapshot_schedule_periodic(u64 interval_seconds, snapshot_type_t type);
snapshot_t **snapshot_list_snapshots(u32 *count);
int snapshot_get_snapshot_details(u64 snapshot_id, snapshot_t *details);
int snapshot_enable_incremental_snapshots(void);
int snapshot_set_retention_policy(u64 max_snapshots, u64 max_age);
int snapshot_export_snapshot(u64 snapshot_id, const char *export_path);
int snapshot_import_snapshot(const char *import_path);
int snapshot_verify_snapshot_integrity(u64 snapshot_id);
int snapshot_compress_old_snapshots(void);

int versioning_enable_for_file(const char *file_path);
int versioning_save_version(const char *file_path, const char *description);
file_version_t **versioning_list_versions(const char *file_path, u32 *count);
int versioning_restore_version(const char *file_path, u64 version_id);
int versioning_delete_version(const char *file_path, u64 version_id);
int versioning_compare_versions(const char *file_path, u64 version1, u64 version2);
int versioning_set_retention_limit(u32 max_versions);
int versioning_enable_auto_versioning(u64 interval_seconds);

#endif
