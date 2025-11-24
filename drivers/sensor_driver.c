#include <drivers/sensor_driver.h>
#include <string.h>

int sensor_driver_init(void) { return 0; }
int sensor_driver_shutdown(void) { return 0; }

int sensor_enumerate_devices(sensor_device_info_t *devices, uint32_t *count, uint32_t max_devices) { return 0; }
sensor_device_t *sensor_open_device(uint32_t device_id) { return 0; }
int sensor_close_device(sensor_device_t *dev) { return 0; }

int sensor_enable_device(sensor_device_t *dev) { return 0; }
int sensor_disable_device(sensor_device_t *dev) { return 0; }
int sensor_is_enabled(sensor_device_t *dev) { return 0; }

int sensor_set_sampling_rate(sensor_device_t *dev, uint32_t rate_hz) { return 0; }
int sensor_get_sampling_rate(sensor_device_t *dev) { return 0; }

int sensor_register_callback(sensor_device_t *dev, sensor_data_callback_fn callback, void *ctx) { return 0; }
int sensor_unregister_callback(sensor_device_t *dev, sensor_data_callback_fn callback) { return 0; }

int sensor_get_device_info(sensor_device_t *dev, sensor_device_info_t *info) { return 0; }

int sensor_camera_start_capture(sensor_device_t *dev) { return 0; }
int sensor_camera_stop_capture(sensor_device_t *dev) { return 0; }
int sensor_camera_set_resolution(sensor_device_t *dev, uint16_t width, uint16_t height) { return 0; }
int sensor_camera_set_frame_rate(sensor_device_t *dev, uint8_t fps) { return 0; }
int sensor_camera_set_zoom(sensor_device_t *dev, float zoom_level) { return 0; }

int sensor_microphone_start_recording(sensor_device_t *dev) { return 0; }
int sensor_microphone_stop_recording(sensor_device_t *dev) { return 0; }
int sensor_microphone_set_sample_rate(sensor_device_t *dev, uint32_t rate) { return 0; }
int sensor_microphone_set_gain(sensor_device_t *dev, int8_t gain_db) { return 0; }

int sensor_gps_start_tracking(sensor_device_t *dev) { return 0; }
int sensor_gps_stop_tracking(sensor_device_t *dev) { return 0; }
int sensor_gps_set_accuracy_level(sensor_device_t *dev, uint16_t accuracy_m) { return 0; }

int sensor_fingerprint_start_enrollment(sensor_device_t *dev) { return 0; }
int sensor_fingerprint_match(sensor_device_t *dev, fingerprint_data_t *data) { return 0; }
int sensor_fingerprint_clear_templates(sensor_device_t *dev) { return 0; }

int sensor_register_permission_checker(sensor_permission_check_fn checker) { return 0; }
int sensor_request_permission(sensor_type_t type, uint32_t pid) { return 0; }
int sensor_revoke_permission(sensor_type_t type, uint32_t pid) { return 0; }
int sensor_has_permission(sensor_type_t type, uint32_t pid) { return 0; }

int sensor_set_privacy_mode(sensor_device_t *dev, uint8_t enabled) { return 0; }
int sensor_get_privacy_mode(sensor_device_t *dev) { return 0; }

int sensor_calibrate_device(sensor_device_t *dev) { return 0; }
int sensor_reset_device(sensor_device_t *dev) { return 0; }

int sensor_get_data_buffered(sensor_device_t *dev, void *buffer, uint32_t buffer_size, uint32_t *data_read) { return 0; }
