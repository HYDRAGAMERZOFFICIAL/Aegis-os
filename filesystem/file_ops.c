#include <filesystem/file_ops.h>
#include <filesystem/encryption.h>
#include <filesystem/snapshots.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_FS_FILES 256
#define MAX_FILE_SIZE 65536

typedef struct {
    char path[256];
    char data[MAX_FILE_SIZE];
    int size;
    int is_open;
    int is_encrypted;
} sim_file_t;

typedef struct {
    char name[128];
    sim_file_t files[MAX_FS_FILES];
    int file_count;
} sim_snapshot_t;

static sim_file_t g_files[MAX_FS_FILES] = {0};
static int g_file_count = 0;
static sim_snapshot_t g_snapshots[16] = {0};
static int g_snapshot_count = 0;
static int g_fs_initialized = 0;

int fs_init(void) {
    g_file_count = 0;
    g_snapshot_count = 0;
    g_fs_initialized = 1;
    return 0;
}

int fs_create_file(const char* path) {
    if (!path || g_file_count >= MAX_FS_FILES) return -1;
    for (int i = 0; i < g_file_count; i++) {
        if (strcmp(g_files[i].path, path) == 0) return i;
    }
    int idx = g_file_count++;
    strncpy(g_files[idx].path, path, 255);
    g_files[idx].size = 0;
    g_files[idx].is_open = 0;
    g_files[idx].is_encrypted = 0;
    return idx;
}

int fs_open_file(const char* path) {
    if (!path) return -1;
    for (int i = 0; i < g_file_count; i++) {
        if (strcmp(g_files[i].path, path) == 0) {
            g_files[i].is_open = 1;
            return i;
        }
    }
    // Auto-create on open if not found
    int idx = fs_create_file(path);
    if (idx >= 0) g_files[idx].is_open = 1;
    return idx;
}

int fs_write_file(int fd, const void* data, int len) {
    if (fd < 0 || fd >= g_file_count || !data || len < 0) return -1;
    if (len > MAX_FILE_SIZE) len = MAX_FILE_SIZE;
    memcpy(g_files[fd].data, data, len);
    g_files[fd].size = len;
    return len;
}

int fs_read_file(int fd, void* data, int len) {
    if (fd < 0 || fd >= g_file_count || !data || len < 0) return -1;
    int to_read = (g_files[fd].size < len) ? g_files[fd].size : len;
    memcpy(data, g_files[fd].data, to_read);
    return to_read;
}

int fs_close_file(int fd) {
    if (fd < 0 || fd >= g_file_count) return -1;
    g_files[fd].is_open = 0;
    return 0;
}

int fs_delete_file(const char* path) {
    if (!path) return -1;
    for (int i = 0; i < g_file_count; i++) {
        if (strcmp(g_files[i].path, path) == 0) {
            for (int j = i; j < g_file_count - 1; j++) {
                g_files[j] = g_files[j + 1];
            }
            g_file_count--;
            return 0;
        }
    }
    return -1;
}

int fs_encrypt_file(const char* path) {
    if (!path) return -1;
    for (int i = 0; i < g_file_count; i++) {
        if (strcmp(g_files[i].path, path) == 0) {
            g_files[i].is_encrypted = 1;
            for (int b = 0; b < g_files[i].size; b++) {
                g_files[i].data[b] ^= 0x5A;
            }
            return 0;
        }
    }
    int idx = fs_create_file(path);
    if (idx >= 0) {
        g_files[idx].is_encrypted = 1;
        return 0;
    }
    return -1;
}

int fs_decrypt_file(const char* path) {
    if (!path) return -1;
    for (int i = 0; i < g_file_count; i++) {
        if (strcmp(g_files[i].path, path) == 0) {
            if (g_files[i].is_encrypted) {
                for (int b = 0; b < g_files[i].size; b++) {
                    g_files[i].data[b] ^= 0x5A;
                }
                g_files[i].is_encrypted = 0;
            }
            return 0;
        }
    }
    return 0;
}

int fs_create_snapshot(const char* name) {
    if (!name || g_snapshot_count >= 16) return -1;
    sim_snapshot_t *snap = &g_snapshots[g_snapshot_count++];
    strncpy(snap->name, name, 127);
    memcpy(snap->files, g_files, sizeof(sim_file_t) * g_file_count);
    snap->file_count = g_file_count;
    return 0;
}

int fs_restore_snapshot(const char* name) {
    if (!name) return -1;
    for (int i = 0; i < g_snapshot_count; i++) {
        if (strcmp(g_snapshots[i].name, name) == 0) {
            memcpy(g_files, g_snapshots[i].files, sizeof(sim_file_t) * g_snapshots[i].file_count);
            g_file_count = g_snapshots[i].file_count;
            return 0;
        }
    }
    return -1;
}
