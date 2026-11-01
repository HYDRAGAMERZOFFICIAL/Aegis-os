#ifndef FILESYSTEM_FILE_OPS_H
#define FILESYSTEM_FILE_OPS_H

#include <stdint.h>
#include <stddef.h>

int fs_init(void);
int fs_create_file(const char* path);
int fs_open_file(const char* path);
int fs_write_file(int fd, const void* data, int len);
int fs_read_file(int fd, void* data, int len);
int fs_close_file(int fd);
int fs_delete_file(const char* path);

#endif

