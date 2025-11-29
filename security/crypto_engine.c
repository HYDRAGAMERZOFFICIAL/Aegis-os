#include <security/crypto_engine.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Simple PRNG state
static u64 g_prng_state = 0xDEADBEEF;

int crypto_engine_init(void) {
  g_prng_state = 123456789;
  return 0;
}

int crypto_engine_detect_hw_acceleration(void) {
  return 0; // No HW acceleration in simulation
}

int crypto_enable_hw_acceleration(void) {
  return -1; // Not supported
}

int crypto_is_hw_accelerated(cipher_type_t cipher) { return 0; }

cipher_context_t *crypto_create_cipher(cipher_type_t type, const u8 *key,
                                       u64 key_size, const u8 *iv) {
  cipher_context_t *ctx = (cipher_context_t *)malloc(sizeof(cipher_context_t));
  if (!ctx)
    return NULL;

  ctx->cipher_type = type;
  if (key)
    memcpy(ctx->key, key, key_size > 32 ? 32 : key_size);
  if (iv)
    memcpy(ctx->iv, iv, 16);

  return ctx;
}

int crypto_destroy_cipher(cipher_context_t *ctx) {
  if (ctx)
    free(ctx);
  return 0;
}

// Simple XOR-Rotate simulation for encryption
int crypto_encrypt(cipher_context_t *ctx, const u8 *plaintext, u64 plen,
                   u8 *ciphertext, u64 *clen) {
  if (!ctx || !plaintext || !ciphertext || !clen)
    return -1;

  for (u64 i = 0; i < plen; i++) {
    u8 key_byte = ctx->key[i % 32];
    u8 iv_byte = ctx->iv[i % 16];
    // Rotate left by 1 and XOR
    u8 val = plaintext[i];
    val = (val << 1) | (val >> 7);
    ciphertext[i] = val ^ key_byte ^ iv_byte;
  }
  *clen = plen;
  return 0;
}

// Reverse of the above for decryption
int crypto_decrypt(cipher_context_t *ctx, const u8 *ciphertext, u64 clen,
                   u8 *plaintext, u64 *plen) {
  if (!ctx || !ciphertext || !plaintext || !plen)
    return -1;

  for (u64 i = 0; i < clen; i++) {
    u8 key_byte = ctx->key[i % 32];
    u8 iv_byte = ctx->iv[i % 16];
    // XOR and Rotate right by 1
    u8 val = ciphertext[i] ^ key_byte ^ iv_byte;
    plaintext[i] = (val >> 1) | (val << 7);
  }
  *plen = clen;
  return 0;
}

// Simple FNV-1a like hash
hash_result_t *crypto_hash(hash_type_t type, const u8 *data, u64 size) {
  hash_result_t *res = (hash_result_t *)malloc(sizeof(hash_result_t));
  if (!res)
    return NULL;

  u64 hash = 14695981039346656037ULL;
  for (u64 i = 0; i < size; i++) {
    hash ^= data[i];
    hash *= 1099511628211ULL;
  }

  // Fill digest with the hash value repeated
  for (int i = 0; i < 8; i++) {
    res->digest[i] = (hash >> (i * 8)) & 0xFF;
  }
  res->digest_length = 32; // Pretend it's SHA-256 size
  return res;
}

int crypto_verify_hash(hash_type_t type, const u8 *data, u64 size,
                       const u8 *hash) {
  hash_result_t *calced = crypto_hash(type, data, size);
  if (!calced)
    return -1;

  int match = memcmp(calced->digest, hash, calced->digest_length);
  free(calced);
  return match == 0 ? 0 : -1;
}

int crypto_generate_random(u8 *buffer, u64 size) {
  if (!buffer)
    return -1;
  for (u64 i = 0; i < size; i++) {
    g_prng_state = g_prng_state * 6364136223846793005ULL + 1;
    buffer[i] = (g_prng_state >> 33) & 0xFF;
  }
  return 0;
}

int crypto_generate_rsa_keypair(asymmetric_algo_t algo, u8 **pub_key,
                                u64 *pub_key_size, u8 **priv_key,
                                u64 *priv_key_size) {
  *pub_key_size = 256;
  *priv_key_size = 256;
  *pub_key = malloc(256);
  *priv_key = malloc(256);
  crypto_generate_random(*pub_key, 256);
  crypto_generate_random(*priv_key, 256);
  return 0;
}

int crypto_rsa_sign(const u8 *priv_key, u64 priv_key_size, const u8 *data,
                    u64 data_size, u8 *signature, u64 *sig_size) {
  *sig_size = 256;
  crypto_generate_random(signature, 256); // Fake signature
  return 0;
}

int crypto_rsa_verify(const u8 *pub_key, u64 pub_key_size, const u8 *data,
                      u64 data_size, const u8 *signature, u64 sig_size) {
  return 0; // Always verify successfully in simulation
}

int crypto_derive_key(void *password, int plen, void *salt, void *derived_key) {
  // Simple PBKDF2 simulation
  if (!password || !derived_key)
    return -1;
  memcpy(derived_key, password, plen > 32 ? 32 : plen);
  return 0;
}
