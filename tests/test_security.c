#include "test_framework.h"
#include <security/crypto_engine.h>
#include <security/microkernel.h>
#include <security/aegis_guard.h>

extern int crypto_init(void);
extern int crypto_aes_encrypt(void* data, int len, void* key);
extern int crypto_aes_decrypt(void* data, int len, void* key);
extern int microkernel_init(void);
extern int microkernel_isolate_driver(int driver_id);
extern int aegis_guard_init(void);
extern int aegis_guard_detect_anomaly(void* behavior);

TEST_SUITE(security) {
    printf("\n=== Security Module Tests ===\n");

    TEST_CASE(crypto_initialization) {
        int result = crypto_init();
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(aes_encryption) {
        uint8_t data[16] = {0};
        uint8_t key[32] = {0};
        int result = crypto_aes_encrypt(data, 16, key);
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(aes_decryption) {
        uint8_t data[16] = {0};
        uint8_t key[32] = {0};
        int result = crypto_aes_decrypt(data, 16, key);
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(microkernel_initialization) {
        int result = microkernel_init();
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(driver_isolation) {
        int result = microkernel_isolate_driver(1);
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(aegis_guard_initialization) {
        int result = aegis_guard_init();
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(anomaly_detection) {
        int behavior = 0;
        int result = aegis_guard_detect_anomaly(&behavior);
        ASSERT_NOT_EQUAL(result, -1);
    } TEST_END();
}
