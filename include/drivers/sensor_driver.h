#ifndef AEGIS_DRIVERS_SENSOR_H
#define AEGIS_DRIVERS_SENSOR_H

#include <kernel/types.h>

typedef enum {
    SENSOR_TYPE_CAMERA = 1,
    SENSOR_TYPE_MICROPHONE = 2,
    SENSOR_TYPE_ACCELEROMETER = 3,
    SENSOR_TYPE_GYROSCOPE = 4,
    SENSOR_TYPE_MAGNETOMETER = 5,
    SENSOR_TYPE_PROXIMITY = 6,
    SENSOR_TYPE_LIGHT = 7,
    SENSOR_TYPE_GPS = 8,
    SENSOR_TYPE_FINGERPRINT = 9,
    SENSOR_TYPE_THERMAL = 10,
} sensor_type_t;

typedef struct {
    char name[64];
    sensor_type_t type;
    uint8_t enabled;
    uint8_t access_granted;
    uint32_t sampling_rate_hz;
    uint32_t buffer_size;
} sensor_device_info_t;

typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
    uint64_t timestamp_us;
} imu_data_t;

typedef struct {
    int16_t light_level;
    int8_t color_temp_k;
    uint64_t timestamp_us;
} light_data_t;

typedef struct {
    int32_t latitude_scaled;
    int32_t longitude_scaled;
    int16_t altitude_m;
    uint16_t accuracy_m;
    uint64_t timestamp_us;
} gps_data_t;

typedef struct {
    uint8_t *frame_data;
    uint32_t frame_size;
    uint16_t width;
    uint16_t height;
    uint8_t format;
    uint64_t timestamp_us;
} camera_frame_t;

typedef struct {
    int16_t *audio_buffer;
    uint32_t sample_count;
    uint32_t sample_rate;
    uint8_t channels;
    uint64_t timestamp_us;
} microphone_data_t;

typedef struct {
    uint32_t match_score;
    uint8_t matched;
    uint64_t timestamp_us;
} fingerprint_data_t;

typedef struct sensor_device sensor_device_t;

typedef void (*sensor_data_callback_fn)(sensor_device_t *dev, void *data, uint32_t size, void *ctx);
typedef int (*sensor_permission_check_fn)(sensor_type_t type, uint32_t pid);

int sensor_driver_init(void);
int sensor_driver_shutdown(void);

int sensor_enumerate_devices(sensor_device_info_t *devices, uint32_t *count, uint32_t max_devices);
sensor_device_t *sensor_open_device(uint32_t device_id);
int sensor_close_device(sensor_device_t *dev);

int sensor_enable_device(sensor_device_t *dev);
int sensor_disable_device(sensor_device_t *dev);
int sensor_is_enabled(sensor_device_t *dev);

int sensor_set_sampling_rate(sensor_device_t *dev, uint32_t rate_hz);
int sensor_get_sampling_rate(sensor_device_t *dev);

int sensor_register_callback(sensor_device_t *dev, sensor_data_callback_fn callback, void *ctx);
int sensor_unregister_callback(sensor_device_t *dev, sensor_data_callback_fn callback);

int sensor_get_device_info(sensor_device_t *dev, sensor_device_info_t *info);

int sensor_camera_start_capture(sensor_device_t *dev);
int sensor_camera_stop_capture(sensor_device_t *dev);
int sensor_camera_set_resolution(sensor_device_t *dev, uint16_t width, uint16_t height);
int sensor_camera_set_frame_rate(sensor_device_t *dev, uint8_t fps);
int sensor_camera_set_zoom(sensor_device_t *dev, float zoom_level);

int sensor_microphone_start_recording(sensor_device_t *dev);
int sensor_microphone_stop_recording(sensor_device_t *dev);
int sensor_microphone_set_sample_rate(sensor_device_t *dev, uint32_t rate);
int sensor_microphone_set_gain(sensor_device_t *dev, int8_t gain_db);

int sensor_gps_start_tracking(sensor_device_t *dev);
int sensor_gps_stop_tracking(sensor_device_t *dev);
int sensor_gps_set_accuracy_level(sensor_device_t *dev, uint16_t accuracy_m);

int sensor_fingerprint_start_enrollment(sensor_device_t *dev);
int sensor_fingerprint_match(sensor_device_t *dev, fingerprint_data_t *data);
int sensor_fingerprint_clear_templates(sensor_device_t *dev);

int sensor_register_permission_checker(sensor_permission_check_fn checker);
int sensor_request_permission(sensor_type_t type, uint32_t pid);
int sensor_revoke_permission(sensor_type_t type, uint32_t pid);
int sensor_has_permission(sensor_type_t type, uint32_t pid);

int sensor_set_privacy_mode(sensor_device_t *dev, uint8_t enabled);
int sensor_get_privacy_mode(sensor_device_t *dev);

int sensor_calibrate_device(sensor_device_t *dev);
int sensor_reset_device(sensor_device_t *dev);

int sensor_get_data_buffered(sensor_device_t *dev, void *buffer, uint32_t buffer_size, uint32_t *data_read);

#endif
