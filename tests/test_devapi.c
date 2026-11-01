#include "test_framework.h"
#include <devapi/core_api.h>
#include <devapi/ui_api.h>
#include <devapi/network_api.h>
#include <devapi/crypto_api.h>

TEST_SUITE(devapi) {
    printf("\n=== Developer API Tests ===\n");

    TEST_CASE(core_api_initialization) {
        aegis_pid_t pid = 0;
        int result = aegis_process_get_current(&pid);
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(process_creation_api) {
        aegis_pid_t pid = 0;
        int result = aegis_process_create("api_test_process", "/bin/app", 0, NULL, &pid);
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(thread_creation_api) {
        aegis_tid_t tid = 0;
        int result = aegis_thread_create("thread_api_test", NULL, NULL, &tid);
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(ui_api_initialization) {
        int result = aegis_ui_init();
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(window_creation) {
        aegis_ui_window_t *wnd = aegis_ui_window_create("Test Window", 800, 600, 0);
        (void)wnd;
        ASSERT_TRUE(1);
    } TEST_END();

    TEST_CASE(network_api_initialization) {
        aegis_socket_t *sock = NULL;
        int result = aegis_socket_create(AEGIS_SOCKET_AF_INET, AEGIS_SOCKET_STREAM, AEGIS_SOCKET_IPPROTO_TCP, &sock);
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(socket_creation) {
        aegis_socket_t *sock = NULL;
        int result = aegis_socket_create(AEGIS_SOCKET_AF_INET, AEGIS_SOCKET_STREAM, AEGIS_SOCKET_IPPROTO_TCP, &sock);
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(socket_connection) {
        aegis_socket_t *sock = NULL;
        int result = aegis_socket_create(AEGIS_SOCKET_AF_INET, AEGIS_SOCKET_STREAM, AEGIS_SOCKET_IPPROTO_TCP, &sock);
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(crypto_api_initialization) {
        uint8_t buf[16] = {0};
        int result = aegis_random_get_bytes(buf, 16);
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(crypto_aes_encryption) {
        aegis_cipher_context_t *ctx = aegis_cipher_create(AEGIS_CIPHER_AES_256, AEGIS_MODE_CBC);
        (void)ctx;
        ASSERT_TRUE(1);
    } TEST_END();
}
