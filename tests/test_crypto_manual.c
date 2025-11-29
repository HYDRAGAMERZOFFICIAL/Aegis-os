#include <assert.h>
#include <security/crypto_engine.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  printf("Testing Crypto Engine...\n");

  // 1. Init
  assert(crypto_engine_init() == 0);
  printf("[PASS] crypto_engine_init\n");

  // 2. Encryption/Decryption
  u8 key[32] = {0x1, 0x2, 0x3, 0x4};
  u8 iv[16] = {0};
  cipher_context_t *ctx = crypto_create_cipher(CIPHER_AES_XTS, key, 32, iv);
  assert(ctx != NULL);

  const char *msg = "Hello Aegis OS!";
  u64 len = strlen(msg);
  u8 encrypted[64];
  u8 decrypted[64];
  u64 clen, plen;

  assert(crypto_encrypt(ctx, (u8 *)msg, len, encrypted, &clen) == 0);
  assert(clen == len);
  assert(memcmp(msg, encrypted, len) != 0); // Should be different
  printf("[PASS] crypto_encrypt\n");

  assert(crypto_decrypt(ctx, encrypted, clen, decrypted, &plen) == 0);
  assert(plen == len);
  decrypted[len] = '\0';
  assert(strcmp(msg, (char *)decrypted) == 0); // Should be same
  printf("[PASS] crypto_decrypt\n");

  crypto_destroy_cipher(ctx);

  // 3. Hashing
  hash_result_t *h1 = crypto_hash(HASH_SHA256, (u8 *)msg, len);
  hash_result_t *h2 = crypto_hash(HASH_SHA256, (u8 *)msg, len);
  assert(h1 != NULL && h2 != NULL);
  assert(memcmp(h1->digest, h2->digest, h1->digest_length) ==
         0); // Deterministic
  printf("[PASS] crypto_hash\n");
  free(h1);
  free(h2);

  // 4. Random
  u8 rnd[16];
  assert(crypto_generate_random(rnd, 16) == 0);
  printf("[PASS] crypto_generate_random\n");

  printf("All tests passed!\n");
  return 0;
}
