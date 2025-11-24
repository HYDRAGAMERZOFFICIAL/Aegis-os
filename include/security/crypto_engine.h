#ifndef AEGIS_CRYPTO_ENGINE_H
#define AEGIS_CRYPTO_ENGINE_H

#include <kernel/types.h>

typedef enum {
    CIPHER_AES_XTS,
    CIPHER_CHACHA20,
    CIPHER_AES_GCM
} cipher_type_t;

typedef enum {
    HASH_SHA256,
    HASH_SHA512,
    HASH_BLAKE2B
} hash_type_t;

typedef enum {
    ASYM_RSA_2048,
    ASYM_RSA_4096,
    ASYM_ECC_P256,
    ASYM_ECC_P384,
    ASYM_KYBER768,
    ASYM_DILITHIUM3
} asymmetric_algo_t;

typedef struct {
    u8 key[32];
    u8 iv[16];
    cipher_type_t cipher_type;
} cipher_context_t;

typedef struct {
    u8 digest[64];
    u64 digest_length;
} hash_result_t;

int crypto_engine_init(void);
int crypto_engine_detect_hw_acceleration(void);
cipher_context_t *crypto_create_cipher(cipher_type_t type, const u8 *key, u64 key_size, const u8 *iv);
int crypto_encrypt(cipher_context_t *ctx, const u8 *plaintext, u64 plen, u8 *ciphertext, u64 *clen);
int crypto_decrypt(cipher_context_t *ctx, const u8 *ciphertext, u64 clen, u8 *plaintext, u64 *plen);
int crypto_destroy_cipher(cipher_context_t *ctx);
hash_result_t *crypto_hash(hash_type_t type, const u8 *data, u64 size);
int crypto_verify_hash(hash_type_t type, const u8 *data, u64 size, const u8 *hash);
int crypto_generate_rsa_keypair(asymmetric_algo_t algo, u8 **pub_key, u64 *pub_key_size, u8 **priv_key, u64 *priv_key_size);
int crypto_rsa_sign(const u8 *priv_key, u64 priv_key_size, const u8 *data, u64 data_size, u8 *signature, u64 *sig_size);
int crypto_rsa_verify(const u8 *pub_key, u64 pub_key_size, const u8 *data, u64 data_size, const u8 *signature, u64 sig_size);
int crypto_generate_random(u8 *buffer, u64 size);
int crypto_enable_hw_acceleration(void);
int crypto_is_hw_accelerated(cipher_type_t cipher);

#endif
