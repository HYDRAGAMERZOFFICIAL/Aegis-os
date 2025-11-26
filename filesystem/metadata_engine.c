#include <filesystem/metadata_engine.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    file_metadata_t *metadata_store;
    u64 metadata_count;
    merkle_tree_node_t *merkle_tree;
    u32 merkle_node_count;
} metadata_engine_state_t;

static metadata_engine_state_t metadata_state = {0};

int metadata_engine_init(void)
{
    metadata_state.metadata_store = (file_metadata_t *)calloc(4096, sizeof(file_metadata_t));
    if (!metadata_state.metadata_store) return -1;

    metadata_state.merkle_tree = (merkle_tree_node_t *)calloc(1024, sizeof(merkle_tree_node_t));
    if (!metadata_state.merkle_tree) return -1;

    metadata_state.metadata_count = 0;
    metadata_state.merkle_node_count = 0;

    return 0;
}

file_metadata_t *metadata_get_file_info(const char *path)
{
    if (!path) return NULL;

    for (u64 i = 0; i < metadata_state.metadata_count; i++) {
        if (strcmp(metadata_state.metadata_store[i].filename, path) == 0) {
            return &metadata_state.metadata_store[i];
        }
    }

    return NULL;
}

int metadata_set_file_info(const char *path, file_metadata_t *metadata)
{
    if (!path || !metadata) return -1;

    file_metadata_t *existing = metadata_get_file_info(path);
    if (existing) {
        memcpy(existing, metadata, sizeof(file_metadata_t));
        return 0;
    }

    if (metadata_state.metadata_count >= 4096) return -1;

    metadata_state.metadata_store[metadata_state.metadata_count] = *metadata;
    metadata_state.metadata_count++;

    return 0;
}

int metadata_update_modified_time(const char *path)
{
    file_metadata_t *info = metadata_get_file_info(path);
    if (!info) return -1;

    info->modified_time = 0;
    return 0;
}

int metadata_update_accessed_time(const char *path)
{
    file_metadata_t *info = metadata_get_file_info(path);
    if (!info) return -1;

    info->accessed_time = 0;
    return 0;
}

merkle_tree_node_t *metadata_compute_merkle_tree(const char *path)
{
    if (!path) return NULL;

    if (metadata_state.merkle_node_count >= 1024) return NULL;

    merkle_tree_node_t *node = &metadata_state.merkle_tree[metadata_state.merkle_node_count];
    node->node_id = metadata_state.merkle_node_count;
    metadata_state.merkle_node_count++;

    return node;
}

int metadata_verify_merkle_tree(const char *path)
{
    if (!path) return -1;
    return 0;
}

int metadata_get_file_hash(const char *path, u8 *hash)
{
    if (!path || !hash) return -1;
    memset(hash, 0, 32);
    return 0;
}

int metadata_update_file_hash(const char *path)
{
    if (!path) return -1;
    return 0;
}

int metadata_list_directory(const char *dir_path, file_metadata_t **files, u32 *count)
{
    if (!dir_path || !files || !count) return -1;

    u32 dir_count = 0;
    file_metadata_t **result = (file_metadata_t **)calloc(256, sizeof(file_metadata_t *));
    if (!result) return -1;

    for (u64 i = 0; i < metadata_state.metadata_count; i++) {
        if (dir_count >= 256) break;
        result[dir_count++] = &metadata_state.metadata_store[i];
    }

    *files = (file_metadata_t *)result;
    *count = dir_count;

    return 0;
}

int metadata_set_ownership(const char *path, u32 uid, u32 gid)
{
    file_metadata_t *info = metadata_get_file_info(path);
    if (!info) return -1;

    info->owner_uid = uid;
    info->owner_gid = gid;

    return 0;
}

int metadata_set_permissions(const char *path, u32 permissions)
{
    file_metadata_t *info = metadata_get_file_info(path);
    if (!info) return -1;

    info->permissions = permissions;

    return 0;
}

u32 metadata_get_permissions(const char *path)
{
    file_metadata_t *info = metadata_get_file_info(path);
    if (!info) return 0;

    return info->permissions;
}
