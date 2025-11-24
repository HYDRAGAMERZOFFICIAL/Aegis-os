#ifndef AEGIS_DRIVERS_INPUT_H
#define AEGIS_DRIVERS_INPUT_H

#include <kernel/types.h>

typedef enum {
    INPUT_TYPE_KEYBOARD = 1,
    INPUT_TYPE_MOUSE = 2,
    INPUT_TYPE_TOUCHPAD = 3,
    INPUT_TYPE_TOUCHSCREEN = 4,
} input_device_type_t;

typedef enum {
    INPUT_EVENT_KEY_PRESS = 1,
    INPUT_EVENT_KEY_RELEASE = 2,
    INPUT_EVENT_MOUSE_MOVE = 3,
    INPUT_EVENT_MOUSE_CLICK = 4,
    INPUT_EVENT_SCROLL = 5,
    INPUT_EVENT_TOUCH = 6,
} input_event_type_t;

typedef struct {
    uint32_t keycode;
    uint8_t flags;
    uint64_t timestamp_us;
} key_event_t;

typedef struct {
    int32_t x;
    int32_t y;
    int32_t delta_x;
    int32_t delta_y;
    uint8_t button_state;
    uint64_t timestamp_us;
} mouse_event_t;

typedef struct {
    uint32_t touch_id;
    int32_t x;
    int32_t y;
    uint8_t pressure;
    input_event_type_t type;
    uint64_t timestamp_us;
} touch_event_t;

typedef struct {
    input_event_type_t type;
    union {
        key_event_t key;
        mouse_event_t mouse;
        touch_event_t touch;
    } data;
} input_event_t;

typedef struct input_device input_device_t;

typedef int (*input_event_handler_fn)(input_event_t *event, void *ctx);

int input_driver_init(void);
int input_driver_shutdown(void);

input_device_t *input_register_device(const char *name, input_device_type_t type);
int input_unregister_device(input_device_t *dev);

int input_enable_device(input_device_t *dev);
int input_disable_device(input_device_t *dev);

int input_post_event(input_device_t *dev, input_event_t *event);

int input_register_handler(input_device_type_t type, input_event_handler_fn handler, void *ctx);
int input_unregister_handler(input_device_type_t type, input_event_handler_fn handler);

int input_set_scan_rate(input_device_t *dev, uint16_t rate_hz);
int input_get_device_info(input_device_t *dev, char *info, uint32_t max_len);

int input_normalize_event(input_event_t *event, input_device_t *dev);
int input_calibrate_device(input_device_t *dev);

int input_enable_low_latency_mode(input_device_t *dev);
int input_disable_low_latency_mode(input_device_t *dev);

uint32_t input_get_pending_events(input_device_t *dev);
int input_flush_events(input_device_t *dev);

#endif
