#ifndef AEGIS_FILE_ENCRYPTION_H
#define AEGIS_FILE_ENCRYPTION_H

#include <kernel/types.h>

typedef enum {
    ENC_ALGORITHM_AES_XTS,
    ENC_ALGORITHM_CHACHA20
} encryption_algorithm_t;

typedef struct {
    u64 file_id;
    const char *file_path;
    bool encrypted;
    encryption_algorithm_t algorithm;
    u8 file_key[32];
    u64 key_id;
    u8 iv[16];
} file_encryption_info_t;

typedef struct {
    u64 key_id;
    u8 key_material[32];
    u64 created_time;
    u64 rotated_time;
    bool is_active;
} encryption_key_t;

int file_encryption_init(void);
int file_encryption_enable(const char *file_path);
int file_encryption_disable(const char *file_path);
int file_encryption_encrypt_file(const char *file_path, const u8 *key);
int file_encryption_decrypt_file(const char *file_path, const u8 *key);
int file_encryption_generate_key(u8 *key, u64 key_size);
int file_encryption_derive_key(const char *password, u8 *derived_key, u64 key_size);
int file_encryption_is_encrypted(const char *file_path);
int file_encryption_get_encryption_info(const char *file_path, file_encryption_info_t *info);
int file_encryption_rotate_keys(void);
int file_encryption_enable_hw_acceleration(void);
int file_encryption_set_default_algorithm(encryption_algorithm_t algo);
int file_encryption_bulk_encrypt_files(const char **paths, u32 count);

#endif
