#ifndef COMMON_CRYPTO_H
#define COMMON_CRYPTO_H

#include <kernel/types.h>

typedef struct {
    u32 state[5];
} sha1_ctx_t;

void sha1_init(sha1_ctx_t *ctx);
void sha1_update(sha1_ctx_t *ctx, const u8 *data, u64 len);
void sha1_final(sha1_ctx_t *ctx, u8 digest[20]);

typedef struct {
    u32 state[8];
} sha256_ctx_t;

void sha256_init(sha256_ctx_t *ctx);
void sha256_update(sha256_ctx_t *ctx, const u8 *data, u64 len);
void sha256_final(sha256_ctx_t *ctx, u8 digest[32]);

int aes_encrypt(const u8 *key, u32 key_size, const u8 *plaintext, u8 *ciphertext, u64 size);

#endif

