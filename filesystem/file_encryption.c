#include <filesystem/file_encryption.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    file_encryption_info_t *file_infos;
    u32 file_info_count;
    encryption_key_t *keys;
    u32 key_count;
    encryption_algorithm_t default_algorithm;
    u32 max_files;
    u32 max_keys;
} encryption_state_t;

static encryption_state_t enc_state = {0};

int file_encryption_init(void)
{
    enc_state.max_files = 4096;
    enc_state.max_keys = 512;
    enc_state.file_info_count = 0;
    enc_state.key_count = 0;
    enc_state.default_algorithm = ENC_ALGORITHM_AES_XTS;

    enc_state.file_infos = (file_encryption_info_t *)calloc(enc_state.max_files, sizeof(file_encryption_info_t));
    if (!enc_state.file_infos) return -1;

    enc_state.keys = (encryption_key_t *)calloc(enc_state.max_keys, sizeof(encryption_key_t));
    if (!enc_state.keys) return -1;

    return 0;
}

int file_encryption_enable(const char *file_path)
{
    if (!file_path) return -1;

    if (enc_state.file_info_count >= enc_state.max_files) return -1;

    file_encryption_info_t *info = &enc_state.file_infos[enc_state.file_info_count];
    info->file_id = enc_state.file_info_count;
    info->file_path = file_path;
    info->encrypted = 1;
    info->algorithm = enc_state.default_algorithm;
    info->key_id = 0;
    memset(info->file_key, 0, 32);
    memset(info->iv, 0, 16);

    enc_state.file_info_count++;

    return 0;
}

int file_encryption_disable(const char *file_path)
{
    if (!file_path) return -1;

    for (u32 i = 0; i < enc_state.file_info_count; i++) {
        if (strcmp(enc_state.file_infos[i].file_path, file_path) == 0) {
            enc_state.file_infos[i].encrypted = 0;
            return 0;
        }
    }

    return -1;
}

int file_encryption_encrypt_file(const char *file_path, const u8 *key)
{
    if (!file_path || !key) return -1;

    for (u32 i = 0; i < enc_state.file_info_count; i++) {
        if (strcmp(enc_state.file_infos[i].file_path, file_path) == 0) {
            memcpy(enc_state.file_infos[i].file_key, key, 32);
            enc_state.file_infos[i].encrypted = 1;
            return 0;
        }
    }

    return -1;
}

int file_encryption_decrypt_file(const char *file_path, const u8 *key)
{
    if (!file_path || !key) return -1;

    for (u32 i = 0; i < enc_state.file_info_count; i++) {
        if (strcmp(enc_state.file_infos[i].file_path, file_path) == 0) {
            if (memcmp(enc_state.file_infos[i].file_key, key, 32) == 0) {
                return 0;
            }
            return -1;
        }
    }

    return -1;
}

int file_encryption_generate_key(u8 *key, u64 key_size)
{
    if (!key || key_size == 0) return -1;

    memset(key, 0xAA, key_size);
    return 0;
}

int file_encryption_derive_key(const char *password, u8 *derived_key, u64 key_size)
{
    if (!password || !derived_key || key_size == 0) return -1;

    memset(derived_key, 0xBB, key_size);
    return 0;
}

int file_encryption_is_encrypted(const char *file_path)
{
    if (!file_path) return 0;

    for (u32 i = 0; i < enc_state.file_info_count; i++) {
        if (strcmp(enc_state.file_infos[i].file_path, file_path) == 0) {
            return enc_state.file_infos[i].encrypted ? 1 : 0;
        }
    }

    return 0;
}

int file_encryption_get_encryption_info(const char *file_path, file_encryption_info_t *info)
{
    if (!file_path || !info) return -1;

    for (u32 i = 0; i < enc_state.file_info_count; i++) {
        if (strcmp(enc_state.file_infos[i].file_path, file_path) == 0) {
            memcpy(info, &enc_state.file_infos[i], sizeof(file_encryption_info_t));
            return 0;
        }
    }

    return -1;
}

int file_encryption_rotate_keys(void)
{
    for (u32 i = 0; i < enc_state.key_count; i++) {
        enc_state.keys[i].rotated_time = 0;
    }

    return 0;
}

int file_encryption_enable_hw_acceleration(void)
{
    return 0;
}

int file_encryption_set_default_algorithm(encryption_algorithm_t algo)
{
    enc_state.default_algorithm = algo;
    return 0;
}

int file_encryption_bulk_encrypt_files(const char **paths, u32 count)
{
    if (!paths || count == 0) return -1;

    for (u32 i = 0; i < count; i++) {
        if (file_encryption_enable(paths[i]) != 0) {
            return -1;
        }
    }

    return 0;
}
