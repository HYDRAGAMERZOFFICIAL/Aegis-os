#ifndef FILESYSTEM_ENCRYPTION_H
#define FILESYSTEM_ENCRYPTION_H

#include <stdint.h>

int fs_encrypt_file(const char* path);
int fs_decrypt_file(const char* path);

#endif

