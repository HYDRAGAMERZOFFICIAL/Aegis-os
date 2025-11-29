#ifndef USERLAND_UI_H
#define USERLAND_UI_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    DISPLAY_CONNECTED = 1,
    DISPLAY_DISCONNECTED = 0,
} DisplayState;

typedef enum {
    PIXEL_FORMAT_RGB888 = 0,
    PIXEL_FORMAT_RGBA8888 = 1,
    PIXEL_FORMAT_RGB565 = 2,
} PixelFormat;

typedef enum {
    WIDGET_BUTTON = 0,
    WIDGET_LABEL = 1,
    WIDGET_TEXTBOX = 2,
    WIDGET_LISTBOX = 3,
    WIDGET_WINDOW = 4,
    WIDGET_PANEL = 5,
} WidgetType;

typedef enum {
    EVENT_MOUSE_CLICK = 0,
    EVENT_KEY_PRESS = 1,
    EVENT_WINDOW_CLOSE = 2,
    EVENT_FOCUS_CHANGE = 3,
} UIEventType;

typedef struct {
    uint16_t width;
    uint16_t height;
    uint16_t x;
    uint16_t y;
} Rect;

typedef struct {
    uint32_t id;
    DisplayState state;
    uint16_t width;
    uint16_t height;
    PixelFormat format;
} Display;

typedef struct {
    uint32_t widget_id;
    WidgetType type;
    Rect bounds;
    char label[256];
    uint32_t parent_id;
    void (*on_click)(uint32_t widget_id);
} Widget;

typedef struct {
    UIEventType type;
    uint32_t widget_id;
    union {
        struct {
            int16_t x;
            int16_t y;
            int button;
        } mouse;
        struct {
            int keycode;
            int modifiers;
        } key;
    } data;
} UIEvent;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} ThemeColor;

typedef struct {
    char name[64];
    ThemeColor primary;
    ThemeColor secondary;
    ThemeColor background;
    ThemeColor text;
} Theme;

int display_init(Display* display, uint16_t width, uint16_t height);
int display_set_resolution(Display* display, uint16_t width, uint16_t height);
int display_get_resolution(Display* display, uint16_t* width, uint16_t* height);
int display_refresh(Display* display);
int display_draw_pixel(Display* display, int x, int y, uint32_t color);
int display_draw_rect(Display* display, Rect* rect, uint32_t color);
int display_clear(Display* display);

int window_manager_init(void);
uint32_t window_create(const char* title, Rect* bounds);
int window_destroy(uint32_t window_id);
int window_set_position(uint32_t window_id, int x, int y);
int window_set_size(uint32_t window_id, int width, int height);
int window_set_focus(uint32_t window_id);

int widget_create(uint32_t* widget_id, WidgetType type, Rect* bounds, const char* label);
int widget_set_parent(uint32_t widget_id, uint32_t parent_id);
int widget_set_text(uint32_t widget_id, const char* text);
int widget_set_callback(uint32_t widget_id, void (*callback)(uint32_t));
int widget_render(uint32_t widget_id);
int widget_destroy(uint32_t widget_id);

int theme_load(const char* theme_name);
int theme_apply(Theme* theme);
int theme_set_color(const char* element, uint32_t color);

int input_handler_init(void);
int input_register_listener(void (*callback)(UIEvent*));
int input_get_event(UIEvent* event);

int font_init(void);
int font_load(const char* font_path);
int font_render_text(Display* display, int x, int y, const char* text);

int app_launcher_init(void);
int app_launcher_add_app(const char* name, const char* exec_path);
int app_launcher_launch(const char* app_name);

int file_browser_init(void);
int file_browser_open(const char* path);
int file_browser_select_file(char* selected_file, size_t size);

int terminal_init(void);
int terminal_execute_command(const char* command);
int terminal_get_output(char* buffer, size_t size);

int system_tray_init(void);
int system_tray_add_icon(const char* icon_path);
int system_tray_set_tooltip(const char* tooltip);

int package_manager_init(void);
int package_manager_install(const char* package_name);
int package_manager_remove(const char* package_name);
int package_manager_update(void);

int settings_app_init(void);
int settings_app_get_value(const char* key, char* value, size_t size);
int settings_app_set_value(const char* key, const char* value);

int system_monitor_init(void);
int system_monitor_get_cpu_usage(float* usage);
int system_monitor_get_memory_usage(float* usage);
int system_monitor_get_temperature(float* temp);

int network_config_init(void);
int network_config_set_ipaddr(const char* interface, const char* ipaddr);
int network_config_set_gateway(const char* gateway);

int storage_manager_init(void);
int storage_manager_format(const char* device, const char* filesystem);
int storage_manager_mount(const char* device, const char* mountpoint);
int storage_manager_unmount(const char* mountpoint);

int user_manager_init(void);
int user_manager_create_user(const char* username);
int user_manager_delete_user(const char* username);
int user_manager_set_permissions(const char* username, uint32_t permissions);

int backup_init(void);
int backup_create(const char* source, const char* dest);
int backup_restore(const char* backup_path, const char* dest);

int system_diagnostics_run(void);
int system_diagnostics_get_report(char* buffer, size_t size);

#endif
