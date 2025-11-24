#include <devapi/sensor_api.h>
#include <string.h>

int aegis_sensor_get_permission(aegis_sensor_type_t type) { return 0; }
int aegis_sensor_request_permission(aegis_sensor_type_t type, const char *reason) { return 0; }
int aegis_sensor_revoke_permission(aegis_sensor_type_t type) { return 0; }

int aegis_sensor_enumerate(aegis_sensor_info_t *sensors, uint32_t *count, uint32_t max_sensors) { return 0; }
aegis_sensor_t *aegis_sensor_open(uint32_t sensor_id) { return 0; }
int aegis_sensor_close(aegis_sensor_t *sensor) { return 0; }

int aegis_sensor_enable(aegis_sensor_t *sensor) { return 0; }
int aegis_sensor_disable(aegis_sensor_t *sensor) { return 0; }
int aegis_sensor_set_sampling_rate(aegis_sensor_t *sensor, uint32_t rate_hz) { return 0; }
int aegis_sensor_register_callback(aegis_sensor_t *sensor, aegis_sensor_data_callback_fn callback, void *ctx) { return 0; }
int aegis_sensor_unregister_callback(aegis_sensor_t *sensor, aegis_sensor_data_callback_fn callback) { return 0; }

int aegis_camera_start_preview(aegis_sensor_t *sensor) { return 0; }
int aegis_camera_stop_preview(aegis_sensor_t *sensor) { return 0; }
int aegis_camera_capture_photo(aegis_sensor_t *sensor, const char *output_path) { return 0; }
int aegis_camera_start_video(aegis_sensor_t *sensor, const char *output_path) { return 0; }
int aegis_camera_stop_video(aegis_sensor_t *sensor) { return 0; }
int aegis_camera_set_resolution(aegis_sensor_t *sensor, uint16_t width, uint16_t height) { return 0; }
int aegis_camera_set_fps(aegis_sensor_t *sensor, uint8_t fps) { return 0; }
int aegis_camera_set_zoom(aegis_sensor_t *sensor, float zoom) { return 0; }
int aegis_camera_set_focus_mode(aegis_sensor_t *sensor, uint8_t mode) { return 0; }
int aegis_camera_set_exposure(aegis_sensor_t *sensor, int16_t exposure_compensation) { return 0; }
int aegis_camera_set_white_balance(aegis_sensor_t *sensor, uint8_t mode) { return 0; }

int aegis_microphone_start_recording(aegis_sensor_t *sensor, const char *output_path) { return 0; }
int aegis_microphone_stop_recording(aegis_sensor_t *sensor) { return 0; }
int aegis_microphone_set_sample_rate(aegis_sensor_t *sensor, uint32_t rate) { return 0; }
int aegis_microphone_set_channels(aegis_sensor_t *sensor, uint8_t channels) { return 0; }
int aegis_microphone_set_gain(aegis_sensor_t *sensor, int8_t gain_db) { return 0; }
int aegis_microphone_enable_noise_cancellation(aegis_sensor_t *sensor) { return 0; }
int aegis_microphone_disable_noise_cancellation(aegis_sensor_t *sensor) { return 0; }
int aegis_microphone_enable_echo_cancellation(aegis_sensor_t *sensor) { return 0; }
int aegis_microphone_disable_echo_cancellation(aegis_sensor_t *sensor) { return 0; }

int aegis_gps_start_tracking(aegis_sensor_t *sensor) { return 0; }
int aegis_gps_stop_tracking(aegis_sensor_t *sensor) { return 0; }
int aegis_gps_get_location(aegis_sensor_t *sensor, double *latitude, double *longitude, 
                           double *altitude, uint32_t *accuracy_m) { return 0; }
int aegis_gps_set_update_interval(aegis_sensor_t *sensor, uint32_t interval_ms) { return 0; }
int aegis_gps_enable_assisted_gps(aegis_sensor_t *sensor) { return 0; }
int aegis_gps_disable_assisted_gps(aegis_sensor_t *sensor) { return 0; }

int aegis_accelerometer_get_data(aegis_sensor_t *sensor, aegis_vector3_t *acceleration) { return 0; }
int aegis_accelerometer_start_motion_detection(aegis_sensor_t *sensor, float threshold_g) { return 0; }
int aegis_accelerometer_stop_motion_detection(aegis_sensor_t *sensor) { return 0; }

int aegis_gyroscope_get_data(aegis_sensor_t *sensor, aegis_vector3_t *rotation_rate) { return 0; }
int aegis_gyroscope_calibrate(aegis_sensor_t *sensor) { return 0; }

int aegis_magnetometer_get_data(aegis_sensor_t *sensor, aegis_vector3_t *magnetic_field) { return 0; }
int aegis_magnetometer_get_heading(aegis_sensor_t *sensor, float *heading_degrees) { return 0; }
int aegis_magnetometer_calibrate(aegis_sensor_t *sensor) { return 0; }

int aegis_proximity_get_distance(aegis_sensor_t *sensor, uint32_t *distance_mm) { return 0; }
int aegis_proximity_set_threshold(aegis_sensor_t *sensor, uint32_t threshold_mm) { return 0; }

int aegis_light_get_level(aegis_sensor_t *sensor, uint16_t *lux) { return 0; }
int aegis_light_get_color_temperature(aegis_sensor_t *sensor, uint16_t *kelvin) { return 0; }

int aegis_fingerprint_enroll(aegis_sensor_t *sensor, uint8_t finger_id) { return 0; }
int aegis_fingerprint_verify(aegis_sensor_t *sensor, uint32_t *match_score, uint8_t *matched) { return 0; }
int aegis_fingerprint_delete_template(aegis_sensor_t *sensor, uint8_t finger_id) { return 0; }
int aegis_fingerprint_delete_all(aegis_sensor_t *sensor) { return 0; }
int aegis_fingerprint_get_template_count(aegis_sensor_t *sensor, uint8_t *count) { return 0; }

int aegis_thermal_get_temperature(aegis_sensor_t *sensor, float *temperature_celsius) { return 0; }
int aegis_thermal_set_threshold(aegis_sensor_t *sensor, float max_temperature_celsius) { return 0; }

int aegis_sensor_set_privacy_indicator(aegis_sensor_t *sensor, uint8_t enabled) { return 0; }
int aegis_sensor_get_privacy_status(aegis_sensor_t *sensor, uint8_t *is_private) { return 0; }

int aegis_sensor_get_info(aegis_sensor_t *sensor, aegis_sensor_info_t *info) { return 0; }
int aegis_sensor_calibrate(aegis_sensor_t *sensor) { return 0; }
int aegis_sensor_reset(aegis_sensor_t *sensor) { return 0; }
