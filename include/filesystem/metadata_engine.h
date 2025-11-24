#ifndef AEGIS_METADATA_ENGINE_H
#define AEGIS_METADATA_ENGINE_H

#include <kernel/types.h>

typedef struct {
    u64 file_id;
    const char *filename;
    u64 size;
    u64 created_time;
    u64 modified_time;
    u64 accessed_time;
    u32 owner_uid;
    u32 owner_gid;
    u32 permissions;
    bool is_directory;
} file_metadata_t;

typedef struct {
    u64 node_id;
    u8 hash[32];
    u64 *child_hashes;
    u32 child_count;
} merkle_tree_node_t;

typedef struct {
    u64 acl_id;
    u64 file_id;
    u32 principal_id;
    u32 permissions;
    u64 expires_at;
} acl_entry_t;

int metadata_engine_init(void);
file_metadata_t *metadata_get_file_info(const char *path);
int metadata_set_file_info(const char *path, file_metadata_t *metadata);
int metadata_update_modified_time(const char *path);
int metadata_update_accessed_time(const char *path);
merkle_tree_node_t *metadata_compute_merkle_tree(const char *path);
int metadata_verify_merkle_tree(const char *path);
int metadata_get_file_hash(const char *path, u8 *hash);
int metadata_update_file_hash(const char *path);
int metadata_list_directory(const char *dir_path, file_metadata_t **files, u32 *count);
int metadata_set_ownership(const char *path, u32 uid, u32 gid);
int metadata_set_permissions(const char *path, u32 permissions);
u32 metadata_get_permissions(const char *path);

#endif
