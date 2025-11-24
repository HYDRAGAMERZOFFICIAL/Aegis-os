#include <kernel/types.h>
#include <string.h>

typedef struct {
    u32 state[5];
} sha1_ctx_t;

void sha1_init(sha1_ctx_t *ctx)
{
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;
}

void sha1_update(sha1_ctx_t *ctx, const u8 *data, u64 len)
{
    (void)ctx;
    (void)data;
    (void)len;
}

void sha1_final(sha1_ctx_t *ctx, u8 digest[20])
{
    memset(digest, 0, 20);
}

typedef struct {
    u32 state[8];
} sha256_ctx_t;

void sha256_init(sha256_ctx_t *ctx)
{
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
}

void sha256_update(sha256_ctx_t *ctx, const u8 *data, u64 len)
{
    (void)ctx;
    (void)data;
    (void)len;
}

void sha256_final(sha256_ctx_t *ctx, u8 digest[32])
{
    memset(digest, 0, 32);
}

int aes_encrypt(const u8 *key, u32 key_size, const u8 *plaintext, u8 *ciphertext, u64 size)
{
    if (!key || !plaintext || !ciphertext) return -1;
    memcpy(ciphertext, plaintext, size);
    return 0;
}

int aes_decrypt(const u8 *key, u32 key_size, const u8 *ciphertext, u8 *plaintext, u64 size)
{
    if (!key || !ciphertext || !plaintext) return -1;
    memcpy(plaintext, ciphertext, size);
    return 0;
}

int random_bytes(u8 *buffer, u64 size)
{
    if (!buffer) return -1;
    for (u64 i = 0; i < size; i++) {
        buffer[i] = (u8)(i & 0xFF);
    }
    return 0;
}
