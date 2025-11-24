#include "test_framework.h"
#include <devapi/core_api.h>
#include <devapi/ui_api.h>
#include <devapi/network_api.h>
#include <devapi/crypto_api.h>

extern int aegis_core_init(void);
extern int aegis_process_create(const char* name);
extern int aegis_thread_create(int pid, void* entry);
extern int aegis_ui_init(void);
extern int aegis_window_create(const char* title, int width, int height);
extern int aegis_network_init(void);
extern int aegis_socket_create(int family, int type);
extern int aegis_socket_connect(int sock, const char* host, int port);
extern int aegis_crypto_init(void);
extern int aegis_crypto_aes_encrypt(void* data, int len);

TEST_SUITE(devapi) {
    printf("\n=== Developer API Tests ===\n");

    TEST_CASE(core_api_initialization) {
        int result = aegis_core_init();
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(process_creation_api) {
        int pid = aegis_process_create("api_test_process");
        ASSERT_NOT_EQUAL(pid, -1);
    } TEST_END();

    TEST_CASE(thread_creation_api) {
        int pid = aegis_process_create("thread_api_test");
        int tid = aegis_thread_create(pid, NULL);
        ASSERT_NOT_EQUAL(tid, -1);
    } TEST_END();

    TEST_CASE(ui_api_initialization) {
        int result = aegis_ui_init();
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(window_creation) {
        int wnd = aegis_window_create("Test Window", 800, 600);
        ASSERT_NOT_EQUAL(wnd, -1);
    } TEST_END();

    TEST_CASE(network_api_initialization) {
        int result = aegis_network_init();
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(socket_creation) {
        int sock = aegis_socket_create(0, 1);
        ASSERT_NOT_EQUAL(sock, -1);
    } TEST_END();

    TEST_CASE(socket_connection) {
        int sock = aegis_socket_create(0, 1);
        if (sock >= 0) {
            int result = aegis_socket_connect(sock, "127.0.0.1", 8080);
            ASSERT_NOT_EQUAL(result, -1);
        } else {
            ASSERT_FALSE(1);
        }
    } TEST_END();

    TEST_CASE(crypto_api_initialization) {
        int result = aegis_crypto_init();
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(aes_encryption_api) {
        uint8_t data[16] = {0};
        int result = aegis_crypto_aes_encrypt(data, 16);
        ASSERT_EQUAL(result, 0);
    } TEST_END();
}
