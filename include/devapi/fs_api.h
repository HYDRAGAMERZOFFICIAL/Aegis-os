#ifndef AEGIS_DEVAPI_FS_H
#define AEGIS_DEVAPI_FS_H

#include <kernel/types.h>

typedef enum {
    AEGIS_FILE_READ = 0x01,
    AEGIS_FILE_WRITE = 0x02,
    AEGIS_FILE_APPEND = 0x04,
    AEGIS_FILE_CREATE = 0x08,
    AEGIS_FILE_TRUNCATE = 0x10,
    AEGIS_FILE_EXCLUSIVE = 0x20,
    AEGIS_FILE_NONBLOCK = 0x40,
} aegis_file_mode_t;

typedef enum {
    AEGIS_FILE_TYPE_REGULAR = 1,
    AEGIS_FILE_TYPE_DIRECTORY = 2,
    AEGIS_FILE_TYPE_SYMLINK = 3,
    AEGIS_FILE_TYPE_FIFO = 4,
    AEGIS_FILE_TYPE_SOCKET = 5,
    AEGIS_FILE_TYPE_DEVICE = 6,
} aegis_file_type_t;

typedef struct {
    char name[256];
    aegis_file_type_t type;
    uint64_t size;
    uint32_t permissions;
    uint64_t created_time;
    uint64_t modified_time;
    uint64_t accessed_time;
    uint8_t encrypted;
    uint32_t owner_uid;
    uint32_t owner_gid;
} aegis_file_stat_t;

typedef struct {
    char *entries;
    uint32_t entry_count;
    uint32_t buffer_size;
} aegis_dir_list_t;

typedef struct aegis_file_handle aegis_file_handle_t;
typedef struct aegis_dir_handle aegis_dir_handle_t;

int aegis_file_open(const char *path, aegis_file_mode_t mode, uint32_t permissions, 
                    aegis_file_handle_t **handle);
int aegis_file_close(aegis_file_handle_t *handle);

int aegis_file_read(aegis_file_handle_t *handle, void *buffer, uint32_t size, uint32_t *bytes_read);
int aegis_file_write(aegis_file_handle_t *handle, const void *buffer, uint32_t size, uint32_t *bytes_written);
int aegis_file_seek(aegis_file_handle_t *handle, int64_t offset, uint8_t whence);
int aegis_file_tell(aegis_file_handle_t *handle, uint64_t *position);
int aegis_file_flush(aegis_file_handle_t *handle);
int aegis_file_truncate(aegis_file_handle_t *handle, uint64_t size);

int aegis_file_stat(const char *path, aegis_file_stat_t *stat);
int aegis_file_fstat(aegis_file_handle_t *handle, aegis_file_stat_t *stat);

int aegis_file_delete(const char *path);
int aegis_file_rename(const char *old_path, const char *new_path);
int aegis_file_copy(const char *src_path, const char *dst_path, uint8_t overwrite);

int aegis_dir_create(const char *path, uint32_t permissions);
int aegis_dir_delete(const char *path);
int aegis_dir_open(const char *path, aegis_dir_handle_t **handle);
int aegis_dir_close(aegis_dir_handle_t *handle);
int aegis_dir_read(aegis_dir_handle_t *handle, aegis_file_stat_t *entry, uint8_t *has_next);
int aegis_dir_list(const char *path, aegis_dir_list_t *list);
int aegis_dir_list_free(aegis_dir_list_t *list);

int aegis_file_set_permissions(const char *path, uint32_t permissions);
int aegis_file_get_permissions(const char *path, uint32_t *permissions);
int aegis_file_set_owner(const char *path, uint32_t uid, uint32_t gid);

int aegis_file_read_metadata(const char *path, char *metadata, uint32_t max_len);
int aegis_file_write_metadata(const char *path, const char *metadata);

int aegis_file_encrypt(const char *path, const uint8_t *key, uint32_t key_len);
int aegis_file_decrypt(aegis_file_handle_t *handle, const uint8_t *key, uint32_t key_len);
int aegis_file_is_encrypted(const char *path);

int aegis_file_create_snapshot(const char *path, const char *snapshot_name);
int aegis_file_list_snapshots(const char *path, char *snapshots, uint32_t max_len);
int aegis_file_restore_snapshot(const char *path, const char *snapshot_name);
int aegis_file_delete_snapshot(const char *path, const char *snapshot_name);

int aegis_file_share(const char *path, uint32_t recipient_uid, uint32_t permissions);
int aegis_file_unshare(const char *path, uint32_t recipient_uid);
int aegis_file_get_sharing_info(const char *path, char *info, uint32_t max_len);

int aegis_file_create_hardlink(const char *existing_path, const char *link_path);
int aegis_file_create_symlink(const char *target_path, const char *link_path);
int aegis_file_read_symlink(const char *link_path, char *target, uint32_t max_len);

int aegis_file_watch(const char *path, uint32_t watch_flags, int *watch_fd);
int aegis_file_unwatch(int watch_fd);

int aegis_path_exists(const char *path);
int aegis_path_is_directory(const char *path);
int aegis_path_is_file(const char *path);
int aegis_path_is_symlink(const char *path);
int aegis_path_get_realpath(const char *path, char *realpath, uint32_t max_len);

int aegis_fs_mount(const char *device, const char *mount_point, const char *fs_type, uint32_t flags);
int aegis_fs_unmount(const char *mount_point, uint8_t force);
int aegis_fs_get_usage(const char *path, uint64_t *total, uint64_t *used, uint64_t *free);

int aegis_file_search_by_name(const char *search_path, const char *pattern, 
                              char *results, uint32_t max_len, uint32_t *count);
int aegis_file_search_by_content(const char *search_path, const char *content, 
                                 char *results, uint32_t max_len, uint32_t *count);

#endif
