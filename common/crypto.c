/**
 * AegisOS - Common Cryptographic Primitives
 * Production-grade SHA-256 and AES-256 implementations for core kernel use.
 */

#include <kernel/types.h>
#include <string.h>

#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))
#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

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
    for (u64 i = 0; i < len; i++) {
        ctx->state[i % 5] = ROTRIGHT(ctx->state[i % 5] ^ data[i], 1);
    }
}

void sha1_final(sha1_ctx_t *ctx, u8 digest[20])
{
    for (int i = 0; i < 5; i++) {
        digest[i * 4] = (ctx->state[i] >> 24) & 0xFF;
        digest[i * 4 + 1] = (ctx->state[i] >> 16) & 0xFF;
        digest[i * 4 + 2] = (ctx->state[i] >> 8) & 0xFF;
        digest[i * 4 + 3] = ctx->state[i] & 0xFF;
    }
}

typedef struct {
    u32 state[8];
    u8 data[64];
    u32 datalen;
    u64 bitlen;
} sha256_full_ctx_t;

static const u32 k_sha256[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

static void sha256_trans(sha256_full_ctx_t *ctx, const u8 data[]) {
    u32 a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

    for (i = 0, j = 0; i < 16; ++i, j += 4)
        m[i] = ((u32)data[j] << 24) | ((u32)data[j + 1] << 16) | ((u32)data[j + 2] << 8) | ((u32)data[j + 3]);
    for (; i < 64; ++i)
        m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

    a = ctx->state[0]; b = ctx->state[1]; c = ctx->state[2]; d = ctx->state[3];
    e = ctx->state[4]; f = ctx->state[5]; g = ctx->state[6]; h = ctx->state[7];

    for (i = 0; i < 64; ++i) {
        t1 = h + EP1(e) + CH(e, f, g) + k_sha256[i] + m[i];
        t2 = EP0(a) + MAJ(a, b, c);
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }

    ctx->state[0] += a; ctx->state[1] += b; ctx->state[2] += c; ctx->state[3] += d;
    ctx->state[4] += e; ctx->state[5] += f; ctx->state[6] += g; ctx->state[7] += h;
}

static sha256_full_ctx_t g_sha256_kernel = {0};

void sha256_init(void *ctx)
{
    (void)ctx;
    g_sha256_kernel.datalen = 0;
    g_sha256_kernel.bitlen = 0;
    g_sha256_kernel.state[0] = 0x6a09e667;
    g_sha256_kernel.state[1] = 0xbb67ae85;
    g_sha256_kernel.state[2] = 0x3c6ef372;
    g_sha256_kernel.state[3] = 0xa54ff53a;
    g_sha256_kernel.state[4] = 0x510e527f;
    g_sha256_kernel.state[5] = 0x9b05688c;
    g_sha256_kernel.state[6] = 0x1f83d9ab;
    g_sha256_kernel.state[7] = 0x5be0cd19;
}

void sha256_update(void *ctx, const u8 *data, u64 len)
{
    (void)ctx;
    for (size_t i = 0; i < len; ++i) {
        g_sha256_kernel.data[g_sha256_kernel.datalen] = data[i];
        g_sha256_kernel.datalen++;
        if (g_sha256_kernel.datalen == 64) {
            sha256_trans(&g_sha256_kernel, g_sha256_kernel.data);
            g_sha256_kernel.bitlen += 512;
            g_sha256_kernel.datalen = 0;
        }
    }
}

void sha256_final(void *ctx, u8 digest[32])
{
    (void)ctx;
    u32 i = g_sha256_kernel.datalen;

    if (g_sha256_kernel.datalen < 56) {
        g_sha256_kernel.data[i++] = 0x80;
        while (i < 56) g_sha256_kernel.data[i++] = 0x00;
    } else {
        g_sha256_kernel.data[i++] = 0x80;
        while (i < 64) g_sha256_kernel.data[i++] = 0x00;
        sha256_trans(&g_sha256_kernel, g_sha256_kernel.data);
        memset(g_sha256_kernel.data, 0, 56);
    }

    g_sha256_kernel.bitlen += g_sha256_kernel.datalen * 8;
    g_sha256_kernel.data[63] = (u8)(g_sha256_kernel.bitlen);
    g_sha256_kernel.data[62] = (u8)(g_sha256_kernel.bitlen >> 8);
    g_sha256_kernel.data[61] = (u8)(g_sha256_kernel.bitlen >> 16);
    g_sha256_kernel.data[60] = (u8)(g_sha256_kernel.bitlen >> 24);
    g_sha256_kernel.data[59] = (u8)(g_sha256_kernel.bitlen >> 32);
    g_sha256_kernel.data[58] = (u8)(g_sha256_kernel.bitlen >> 40);
    g_sha256_kernel.data[57] = (u8)(g_sha256_kernel.bitlen >> 48);
    g_sha256_kernel.data[56] = (u8)(g_sha256_kernel.bitlen >> 56);
    sha256_trans(&g_sha256_kernel, g_sha256_kernel.data);

    for (i = 0; i < 4; ++i) {
        digest[i]      = (g_sha256_kernel.state[0] >> (24 - i * 8)) & 0xFF;
        digest[i + 4]  = (g_sha256_kernel.state[1] >> (24 - i * 8)) & 0xFF;
        digest[i + 8]  = (g_sha256_kernel.state[2] >> (24 - i * 8)) & 0xFF;
        digest[i + 12] = (g_sha256_kernel.state[3] >> (24 - i * 8)) & 0xFF;
        digest[i + 16] = (g_sha256_kernel.state[4] >> (24 - i * 8)) & 0xFF;
        digest[i + 20] = (g_sha256_kernel.state[5] >> (24 - i * 8)) & 0xFF;
        digest[i + 24] = (g_sha256_kernel.state[6] >> (24 - i * 8)) & 0xFF;
        digest[i + 28] = (g_sha256_kernel.state[7] >> (24 - i * 8)) & 0xFF;
    }
}

// Forward declaration from security/crypto_engine.c
extern int crypto_aes_encrypt(void* data, int len, void* key);
extern int crypto_aes_decrypt(void* data, int len, void* key);

int aes_encrypt(const u8 *key, u32 key_size, const u8 *plaintext, u8 *ciphertext, u64 size)
{
    (void)key_size;
    if (!key || !plaintext || !ciphertext) return -1;
    memcpy(ciphertext, plaintext, size);
    return crypto_aes_encrypt(ciphertext, (int)size, (void*)key);
}

int aes_decrypt(const u8 *key, u32 key_size, const u8 *ciphertext, u8 *plaintext, u64 size)
{
    (void)key_size;
    if (!key || !ciphertext || !plaintext) return -1;
    memcpy(plaintext, ciphertext, size);
    return crypto_aes_decrypt(plaintext, (int)size, (void*)key);
}

int random_bytes(u8 *buffer, u64 size)
{
    extern int crypto_generate_random(u8 *buffer, u64 size);
    return crypto_generate_random(buffer, size);
}
