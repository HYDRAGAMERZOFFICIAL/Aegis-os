#include <devapi/crypto_api.h>
#include <string.h>

int aegis_random_get_bytes(uint8_t *buffer, uint32_t size) { return 0; }
int aegis_random_get_range(uint32_t min, uint32_t max, uint32_t *result) { return 0; }

aegis_cipher_context_t *aegis_cipher_create(aegis_cipher_type_t cipher, aegis_cipher_mode_t mode) { return 0; }
int aegis_cipher_destroy(aegis_cipher_context_t *ctx) { return 0; }

int aegis_cipher_set_key(aegis_cipher_context_t *ctx, const uint8_t *key, uint32_t key_len) { return 0; }
int aegis_cipher_set_iv(aegis_cipher_context_t *ctx, const uint8_t *iv, uint32_t iv_len) { return 0; }

int aegis_cipher_encrypt(aegis_cipher_context_t *ctx, const uint8_t *plaintext, 
                         uint32_t plaintext_len, uint8_t *ciphertext, uint32_t *ciphertext_len) { return 0; }
int aegis_cipher_decrypt(aegis_cipher_context_t *ctx, const uint8_t *ciphertext,
                         uint32_t ciphertext_len, uint8_t *plaintext, uint32_t *plaintext_len) { return 0; }

int aegis_cipher_encrypt_file(const char *input_file, const char *output_file, 
                              aegis_cipher_type_t cipher, const uint8_t *key, uint32_t key_len) { return 0; }
int aegis_cipher_decrypt_file(const char *input_file, const char *output_file,
                              aegis_cipher_type_t cipher, const uint8_t *key, uint32_t key_len) { return 0; }

aegis_hash_context_t *aegis_hash_create(aegis_hash_type_t hash_type) { return 0; }
int aegis_hash_destroy(aegis_hash_context_t *ctx) { return 0; }

int aegis_hash_update(aegis_hash_context_t *ctx, const uint8_t *data, uint32_t data_len) { return 0; }
int aegis_hash_finalize(aegis_hash_context_t *ctx, uint8_t *hash, uint32_t *hash_len) { return 0; }

int aegis_hash_buffer(aegis_hash_type_t hash_type, const uint8_t *data, uint32_t data_len,
                      uint8_t *hash, uint32_t *hash_len) { return 0; }
int aegis_hash_file(aegis_hash_type_t hash_type, const char *file_path, 
                    uint8_t *hash, uint32_t *hash_len) { return 0; }

int aegis_hmac_compute(aegis_hash_type_t hash_type, const uint8_t *key, uint32_t key_len,
                       const uint8_t *data, uint32_t data_len, uint8_t *hmac, uint32_t *hmac_len) { return 0; }
int aegis_hmac_verify(aegis_hash_type_t hash_type, const uint8_t *key, uint32_t key_len,
                      const uint8_t *data, uint32_t data_len, const uint8_t *hmac, uint32_t hmac_len) { return 0; }

int aegis_key_pair_generate(aegis_asymmetric_type_t key_type, uint32_t key_bits, aegis_key_pair_t **key_pair) { return 0; }
int aegis_key_pair_destroy(aegis_key_pair_t *key_pair) { return 0; }

int aegis_key_pair_export_public(aegis_key_pair_t *key_pair, uint8_t *key_data, uint32_t *key_len) { return 0; }
int aegis_key_pair_export_private(aegis_key_pair_t *key_pair, uint8_t *key_data, uint32_t *key_len,
                                  const uint8_t *password, uint32_t password_len) { return 0; }
int aegis_key_pair_import_public(aegis_asymmetric_type_t key_type, const uint8_t *key_data,
                                 uint32_t key_len, aegis_key_pair_t **key_pair) { return 0; }
int aegis_key_pair_import_private(aegis_asymmetric_type_t key_type, const uint8_t *key_data,
                                  uint32_t key_len, const uint8_t *password, 
                                  uint32_t password_len, aegis_key_pair_t **key_pair) { return 0; }

int aegis_sign_data(aegis_key_pair_t *key_pair, const uint8_t *data, uint32_t data_len,
                    uint8_t *signature, uint32_t *signature_len) { return 0; }
int aegis_verify_signature(aegis_key_pair_t *key_pair, const uint8_t *data, uint32_t data_len,
                           const uint8_t *signature, uint32_t signature_len) { return 0; }

int aegis_encrypt_rsa(aegis_key_pair_t *key_pair, const uint8_t *plaintext, uint32_t plaintext_len,
                      uint8_t *ciphertext, uint32_t *ciphertext_len, aegis_padding_type_t padding) { return 0; }
int aegis_decrypt_rsa(aegis_key_pair_t *key_pair, const uint8_t *ciphertext, uint32_t ciphertext_len,
                      uint8_t *plaintext, uint32_t *plaintext_len, aegis_padding_type_t padding) { return 0; }

int aegis_derive_key(const uint8_t *password, uint32_t password_len,
                     const uint8_t *salt, uint32_t salt_len,
                     uint32_t iterations, uint8_t *derived_key, uint32_t derived_key_len) { return 0; }

int aegis_base64_encode(const uint8_t *data, uint32_t data_len, char *encoded, uint32_t *encoded_len) { return 0; }
int aegis_base64_decode(const char *encoded, uint32_t encoded_len, uint8_t *data, uint32_t *data_len) { return 0; }

int aegis_key_agreement(aegis_key_pair_t *private_key, aegis_key_pair_t *public_key,
                        uint8_t *shared_secret, uint32_t *shared_secret_len) { return 0; }

int aegis_generate_certificate(aegis_key_pair_t *key_pair, const char *subject, const char *issuer,
                               uint32_t validity_days, uint8_t *cert, uint32_t *cert_len) { return 0; }

uint32_t aegis_get_cipher_key_size(aegis_cipher_type_t cipher) { return 0; }
uint32_t aegis_get_cipher_block_size(aegis_cipher_type_t cipher) { return 0; }
uint32_t aegis_get_hash_digest_size(aegis_hash_type_t hash_type) { return 0; }
