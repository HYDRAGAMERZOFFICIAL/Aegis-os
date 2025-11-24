#ifndef AEGIS_DEVAPI_SENSOR_H
#define AEGIS_DEVAPI_SENSOR_H

#include <kernel/types.h>

typedef enum {
    AEGIS_SENSOR_CAMERA = 1,
    AEGIS_SENSOR_MICROPHONE = 2,
    AEGIS_SENSOR_GPS = 3,
    AEGIS_SENSOR_ACCELEROMETER = 4,
    AEGIS_SENSOR_GYROSCOPE = 5,
    AEGIS_SENSOR_MAGNETOMETER = 6,
    AEGIS_SENSOR_PROXIMITY = 7,
    AEGIS_SENSOR_LIGHT = 8,
    AEGIS_SENSOR_FINGERPRINT = 9,
    AEGIS_SENSOR_THERMAL = 10,
} aegis_sensor_type_t;

typedef enum {
    AEGIS_PERMISSION_CAMERA = 0x01,
    AEGIS_PERMISSION_MICROPHONE = 0x02,
    AEGIS_PERMISSION_GPS = 0x04,
    AEGIS_PERMISSION_ACCELEROMETER = 0x08,
    AEGIS_PERMISSION_GYROSCOPE = 0x10,
    AEGIS_PERMISSION_MAGNETOMETER = 0x20,
    AEGIS_PERMISSION_PROXIMITY = 0x40,
    AEGIS_PERMISSION_LIGHT = 0x80,
    AEGIS_PERMISSION_FINGERPRINT = 0x100,
} aegis_sensor_permission_t;

typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
} aegis_vector3_t;

typedef struct {
    int16_t x;
    int16_t y;
} aegis_vector2_t;

typedef struct {
    char device_name[64];
    aegis_sensor_type_t type;
    uint32_t sampling_rate;
    uint8_t enabled;
    uint8_t has_permission;
} aegis_sensor_info_t;

typedef struct aegis_sensor aegis_sensor_t;
typedef void (*aegis_sensor_data_callback_fn)(aegis_sensor_t *sensor, void *data, uint32_t size, void *ctx);

int aegis_sensor_get_permission(aegis_sensor_type_t type);
int aegis_sensor_request_permission(aegis_sensor_type_t type, const char *reason);
int aegis_sensor_revoke_permission(aegis_sensor_type_t type);

int aegis_sensor_enumerate(aegis_sensor_info_t *sensors, uint32_t *count, uint32_t max_sensors);
aegis_sensor_t *aegis_sensor_open(uint32_t sensor_id);
int aegis_sensor_close(aegis_sensor_t *sensor);

int aegis_sensor_enable(aegis_sensor_t *sensor);
int aegis_sensor_disable(aegis_sensor_t *sensor);
int aegis_sensor_set_sampling_rate(aegis_sensor_t *sensor, uint32_t rate_hz);
int aegis_sensor_register_callback(aegis_sensor_t *sensor, aegis_sensor_data_callback_fn callback, void *ctx);
int aegis_sensor_unregister_callback(aegis_sensor_t *sensor, aegis_sensor_data_callback_fn callback);

int aegis_camera_start_preview(aegis_sensor_t *sensor);
int aegis_camera_stop_preview(aegis_sensor_t *sensor);
int aegis_camera_capture_photo(aegis_sensor_t *sensor, const char *output_path);
int aegis_camera_start_video(aegis_sensor_t *sensor, const char *output_path);
int aegis_camera_stop_video(aegis_sensor_t *sensor);
int aegis_camera_set_resolution(aegis_sensor_t *sensor, uint16_t width, uint16_t height);
int aegis_camera_set_fps(aegis_sensor_t *sensor, uint8_t fps);
int aegis_camera_set_zoom(aegis_sensor_t *sensor, float zoom);
int aegis_camera_set_focus_mode(aegis_sensor_t *sensor, uint8_t mode);
int aegis_camera_set_exposure(aegis_sensor_t *sensor, int16_t exposure_compensation);
int aegis_camera_set_white_balance(aegis_sensor_t *sensor, uint8_t mode);

int aegis_microphone_start_recording(aegis_sensor_t *sensor, const char *output_path);
int aegis_microphone_stop_recording(aegis_sensor_t *sensor);
int aegis_microphone_set_sample_rate(aegis_sensor_t *sensor, uint32_t rate);
int aegis_microphone_set_channels(aegis_sensor_t *sensor, uint8_t channels);
int aegis_microphone_set_gain(aegis_sensor_t *sensor, int8_t gain_db);
int aegis_microphone_enable_noise_cancellation(aegis_sensor_t *sensor);
int aegis_microphone_disable_noise_cancellation(aegis_sensor_t *sensor);
int aegis_microphone_enable_echo_cancellation(aegis_sensor_t *sensor);
int aegis_microphone_disable_echo_cancellation(aegis_sensor_t *sensor);

int aegis_gps_start_tracking(aegis_sensor_t *sensor);
int aegis_gps_stop_tracking(aegis_sensor_t *sensor);
int aegis_gps_get_location(aegis_sensor_t *sensor, double *latitude, double *longitude, 
                           double *altitude, uint32_t *accuracy_m);
int aegis_gps_set_update_interval(aegis_sensor_t *sensor, uint32_t interval_ms);
int aegis_gps_enable_assisted_gps(aegis_sensor_t *sensor);
int aegis_gps_disable_assisted_gps(aegis_sensor_t *sensor);

int aegis_accelerometer_get_data(aegis_sensor_t *sensor, aegis_vector3_t *acceleration);
int aegis_accelerometer_start_motion_detection(aegis_sensor_t *sensor, float threshold_g);
int aegis_accelerometer_stop_motion_detection(aegis_sensor_t *sensor);

int aegis_gyroscope_get_data(aegis_sensor_t *sensor, aegis_vector3_t *rotation_rate);
int aegis_gyroscope_calibrate(aegis_sensor_t *sensor);

int aegis_magnetometer_get_data(aegis_sensor_t *sensor, aegis_vector3_t *magnetic_field);
int aegis_magnetometer_get_heading(aegis_sensor_t *sensor, float *heading_degrees);
int aegis_magnetometer_calibrate(aegis_sensor_t *sensor);

int aegis_proximity_get_distance(aegis_sensor_t *sensor, uint32_t *distance_mm);
int aegis_proximity_set_threshold(aegis_sensor_t *sensor, uint32_t threshold_mm);

int aegis_light_get_level(aegis_sensor_t *sensor, uint16_t *lux);
int aegis_light_get_color_temperature(aegis_sensor_t *sensor, uint16_t *kelvin);

int aegis_fingerprint_enroll(aegis_sensor_t *sensor, uint8_t finger_id);
int aegis_fingerprint_verify(aegis_sensor_t *sensor, uint32_t *match_score, uint8_t *matched);
int aegis_fingerprint_delete_template(aegis_sensor_t *sensor, uint8_t finger_id);
int aegis_fingerprint_delete_all(aegis_sensor_t *sensor);
int aegis_fingerprint_get_template_count(aegis_sensor_t *sensor, uint8_t *count);

int aegis_thermal_get_temperature(aegis_sensor_t *sensor, float *temperature_celsius);
int aegis_thermal_set_threshold(aegis_sensor_t *sensor, float max_temperature_celsius);

int aegis_sensor_set_privacy_indicator(aegis_sensor_t *sensor, uint8_t enabled);
int aegis_sensor_get_privacy_status(aegis_sensor_t *sensor, uint8_t *is_private);

int aegis_sensor_get_info(aegis_sensor_t *sensor, aegis_sensor_info_t *info);
int aegis_sensor_calibrate(aegis_sensor_t *sensor);
int aegis_sensor_reset(aegis_sensor_t *sensor);

#endif
