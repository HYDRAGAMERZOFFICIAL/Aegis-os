#include <drivers/input_driver.h>
#include <string.h>

int input_driver_init(void) { return 0; }
int input_driver_shutdown(void) { return 0; }

input_device_t *input_register_device(const char *name, input_device_type_t type) { return 0; }
int input_unregister_device(input_device_t *dev) { return 0; }

int input_enable_device(input_device_t *dev) { return 0; }
int input_disable_device(input_device_t *dev) { return 0; }

int input_post_event(input_device_t *dev, input_event_t *event) { return 0; }

int input_register_handler(input_device_type_t type, input_event_handler_fn handler, void *ctx) { return 0; }
int input_unregister_handler(input_device_type_t type, input_event_handler_fn handler) { return 0; }

int input_set_scan_rate(input_device_t *dev, uint16_t rate_hz) { return 0; }
int input_get_device_info(input_device_t *dev, char *info, uint32_t max_len) { return 0; }

int input_normalize_event(input_event_t *event, input_device_t *dev) { return 0; }
int input_calibrate_device(input_device_t *dev) { return 0; }

int input_enable_low_latency_mode(input_device_t *dev) { return 0; }
int input_disable_low_latency_mode(input_device_t *dev) { return 0; }

uint32_t input_get_pending_events(input_device_t *dev) { return 0; }
int input_flush_events(input_device_t *dev) { return 0; }
