#include "test_framework.h"
#include <drivers/input_driver.h>
#include <drivers/gpu_driver.h>
#include <drivers/storage_driver.h>
#include <drivers/network_driver.h>
#include <drivers/sensor_driver.h>

extern int input_driver_init(void);
extern int input_driver_enumerate_devices(void);
extern int gpu_driver_init(void);
extern int gpu_driver_set_mode(int width, int height, int bpp);
extern int storage_driver_init(void);
extern int storage_driver_enumerate_devices(void);
extern int network_driver_init(void);
extern int network_driver_connect(const char* ssid);
extern int sensor_driver_init(void);
extern int sensor_driver_request_permission(int sensor_type);

TEST_SUITE(drivers) {
    printf("\n=== Driver Module Tests ===\n");

    TEST_CASE(input_driver_initialization) {
        int result = input_driver_init();
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(input_device_enumeration) {
        int result = input_driver_enumerate_devices();
        ASSERT_NOT_EQUAL(result, -1);
    } TEST_END();

    TEST_CASE(gpu_driver_initialization) {
        int result = gpu_driver_init();
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(gpu_driver_set_display_mode) {
        int result = gpu_driver_set_mode(1920, 1080, 32);
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(storage_driver_initialization) {
        int result = storage_driver_init();
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(storage_device_enumeration) {
        int result = storage_driver_enumerate_devices();
        ASSERT_NOT_EQUAL(result, -1);
    } TEST_END();

    TEST_CASE(network_driver_initialization) {
        int result = network_driver_init();
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(sensor_driver_initialization) {
        int result = sensor_driver_init();
        ASSERT_EQUAL(result, 0);
    } TEST_END();

    TEST_CASE(sensor_permission_request) {
        int result = sensor_driver_request_permission(0);
        ASSERT_NOT_EQUAL(result, -1);
    } TEST_END();
}
