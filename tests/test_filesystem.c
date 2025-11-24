#include "test_framework.h"
#include <filesystem/file_ops.h>
#include <filesystem/encryption.h>
#include <filesystem/snapshots.h>

extern int fs_init(void);
extern int fs_create_file(const char* path);
extern int fs_open_file(const char* path);
extern int fs_write_file(int fd, const void* data, int len);
extern int fs_read_file(int fd, void* data, int len);
extern int fs_close_file(int fd);
extern int fs_delete_file(const char* path);
extern int fs_encrypt_file(const char* path);
extern int fs_decrypt_file(const char* path);
extern int fs_create_snapshot(const char* name);
extern int fs_restore_snapshot(const char* name);

TEST_SUITE(filesystem) {
    printf("\n=== Filesystem Module Tests ===\n");

    TEST_CASE(filesystem_initialization) {
        int result = fs_init();
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(file_creation) {
        int result = fs_create_file("/test/file.txt");
        ASSERT_NOT_EQUAL(result, -1);
    } TEST_END();

    TEST_CASE(file_open) {
        fs_create_file("/test/open_test.txt");
        int fd = fs_open_file("/test/open_test.txt");
        ASSERT_NOT_EQUAL(fd, -1);
    } TEST_END();

    TEST_CASE(file_write) {
        int fd = fs_open_file("/test/write_test.txt");
        if (fd >= 0) {
            const char* data = "test data";
            int result = fs_write_file(fd, data, 9);
            ASSERT_EQUAL(result, 9);
            fs_close_file(fd);
        } else {
            ASSERT_FALSE(1);
        }
    } TEST_END();

    TEST_CASE(file_read) {
        char buffer[256] = {0};
        int fd = fs_open_file("/test/read_test.txt");
        if (fd >= 0) {
            int result = fs_read_file(fd, buffer, 256);
            ASSERT_NOT_EQUAL(result, -1);
            fs_close_file(fd);
        } else {
            ASSERT_FALSE(1);
        }
    } TEST_END();

    TEST_CASE(file_encryption) {
        int result = fs_encrypt_file("/test/secret.txt");
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(file_decryption) {
        int result = fs_decrypt_file("/test/secret.txt");
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(snapshot_creation) {
        int result = fs_create_snapshot("backup_1");
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(snapshot_restoration) {
        int result = fs_restore_snapshot("backup_1");
        ASSERT_EQUAL(result, 0);
    } TEST_END();
}
