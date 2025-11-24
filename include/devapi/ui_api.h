#ifndef AEGIS_DEVAPI_UI_H
#define AEGIS_DEVAPI_UI_H

#include <kernel/types.h>

typedef enum {
    AEGIS_UI_BUTTON = 1,
    AEGIS_UI_LABEL = 2,
    AEGIS_UI_TEXT_INPUT = 3,
    AEGIS_UI_WINDOW = 4,
    AEGIS_UI_CONTAINER = 5,
    AEGIS_UI_CHECKBOX = 6,
    AEGIS_UI_RADIO = 7,
    AEGIS_UI_DROPDOWN = 8,
    AEGIS_UI_LISTBOX = 9,
    AEGIS_UI_PROGRESS_BAR = 10,
    AEGIS_UI_SLIDER = 11,
    AEGIS_UI_IMAGE = 12,
    AEGIS_UI_CANVAS = 13,
    AEGIS_UI_MENU = 14,
    AEGIS_UI_TOOLBAR = 15,
} aegis_ui_component_type_t;

typedef enum {
    AEGIS_UI_ALIGN_LEFT = 0,
    AEGIS_UI_ALIGN_CENTER = 1,
    AEGIS_UI_ALIGN_RIGHT = 2,
} aegis_ui_alignment_t;

typedef struct {
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
} aegis_ui_rect_t;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} aegis_ui_color_t;

typedef struct {
    char font_name[64];
    uint16_t font_size;
    uint8_t bold;
    uint8_t italic;
} aegis_ui_font_t;

typedef struct aegis_ui_component aegis_ui_component_t;
typedef struct aegis_ui_window aegis_ui_window_t;

typedef void (*aegis_ui_event_handler_fn)(aegis_ui_component_t *comp, uint32_t event_type, void *event_data, void *ctx);
typedef void (*aegis_ui_animation_fn)(aegis_ui_component_t *comp, float progress, void *ctx);

int aegis_ui_init(void);
int aegis_ui_shutdown(void);
int aegis_ui_run_event_loop(void);
int aegis_ui_quit_event_loop(void);

aegis_ui_window_t *aegis_ui_window_create(const char *title, uint32_t width, uint32_t height, uint32_t style);
int aegis_ui_window_destroy(aegis_ui_window_t *window);
int aegis_ui_window_show(aegis_ui_window_t *window);
int aegis_ui_window_hide(aegis_ui_window_t *window);
int aegis_ui_window_set_size(aegis_ui_window_t *window, uint32_t width, uint32_t height);
int aegis_ui_window_get_size(aegis_ui_window_t *window, uint32_t *width, uint32_t *height);
int aegis_ui_window_set_title(aegis_ui_window_t *window, const char *title);
int aegis_ui_window_focus(aegis_ui_window_t *window);
int aegis_ui_window_is_focused(aegis_ui_window_t *window);

aegis_ui_component_t *aegis_ui_component_create(aegis_ui_window_t *window, 
                                                 aegis_ui_component_type_t type);
int aegis_ui_component_destroy(aegis_ui_component_t *comp);
int aegis_ui_component_set_position(aegis_ui_component_t *comp, int32_t x, int32_t y);
int aegis_ui_component_set_size(aegis_ui_component_t *comp, uint32_t width, uint32_t height);
int aegis_ui_component_get_position(aegis_ui_component_t *comp, int32_t *x, int32_t *y);
int aegis_ui_component_get_size(aegis_ui_component_t *comp, uint32_t *width, uint32_t *height);
int aegis_ui_component_set_visible(aegis_ui_component_t *comp, uint8_t visible);
int aegis_ui_component_get_visible(aegis_ui_component_t *comp);
int aegis_ui_component_set_enabled(aegis_ui_component_t *comp, uint8_t enabled);
int aegis_ui_component_get_enabled(aegis_ui_component_t *comp);

int aegis_ui_component_set_event_handler(aegis_ui_component_t *comp, 
                                         aegis_ui_event_handler_fn handler, void *ctx);
int aegis_ui_component_post_event(aegis_ui_component_t *comp, uint32_t event_type, void *event_data);

int aegis_ui_button_set_label(aegis_ui_component_t *button, const char *label);
int aegis_ui_button_get_label(aegis_ui_component_t *button, char *label, uint32_t max_len);

int aegis_ui_label_set_text(aegis_ui_component_t *label, const char *text);
int aegis_ui_label_get_text(aegis_ui_component_t *label, char *text, uint32_t max_len);
int aegis_ui_label_set_font(aegis_ui_component_t *label, const aegis_ui_font_t *font);

int aegis_ui_text_input_set_text(aegis_ui_component_t *input, const char *text);
int aegis_ui_text_input_get_text(aegis_ui_component_t *input, char *text, uint32_t max_len);
int aegis_ui_text_input_set_placeholder(aegis_ui_component_t *input, const char *placeholder);
int aegis_ui_text_input_set_password_mode(aegis_ui_component_t *input, uint8_t enabled);

int aegis_ui_checkbox_set_checked(aegis_ui_component_t *checkbox, uint8_t checked);
int aegis_ui_checkbox_get_checked(aegis_ui_component_t *checkbox);

int aegis_ui_progress_bar_set_value(aegis_ui_component_t *bar, uint32_t value);
int aegis_ui_progress_bar_set_max(aegis_ui_component_t *bar, uint32_t max);
int aegis_ui_progress_bar_get_value(aegis_ui_component_t *bar);

int aegis_ui_slider_set_value(aegis_ui_component_t *slider, int32_t value);
int aegis_ui_slider_set_range(aegis_ui_component_t *slider, int32_t min, int32_t max);
int aegis_ui_slider_get_value(aegis_ui_component_t *slider);

int aegis_ui_image_set_source(aegis_ui_component_t *image, const char *file_path);
int aegis_ui_image_set_data(aegis_ui_component_t *image, const uint8_t *data, 
                            uint32_t width, uint32_t height, uint8_t format);

int aegis_ui_canvas_clear(aegis_ui_component_t *canvas, aegis_ui_color_t color);
int aegis_ui_canvas_draw_line(aegis_ui_component_t *canvas, int32_t x1, int32_t y1, 
                              int32_t x2, int32_t y2, aegis_ui_color_t color, uint8_t thickness);
int aegis_ui_canvas_draw_rect(aegis_ui_component_t *canvas, const aegis_ui_rect_t *rect, 
                              aegis_ui_color_t color, uint8_t filled);
int aegis_ui_canvas_draw_circle(aegis_ui_component_t *canvas, int32_t cx, int32_t cy, 
                                uint32_t radius, aegis_ui_color_t color, uint8_t filled);
int aegis_ui_canvas_draw_text(aegis_ui_component_t *canvas, const char *text, 
                              int32_t x, int32_t y, aegis_ui_color_t color, const aegis_ui_font_t *font);

int aegis_ui_animate_property(aegis_ui_component_t *comp, const char *property, 
                              void *target_value, uint32_t duration_ms, 
                              aegis_ui_animation_fn callback, void *ctx);
int aegis_ui_animation_cancel(aegis_ui_component_t *comp);

int aegis_ui_show_dialog(aegis_ui_window_t *parent, const char *title, const char *message, 
                         uint32_t dialog_type, int *result);
int aegis_ui_show_file_dialog(aegis_ui_window_t *parent, const char *title, uint32_t mode, 
                              char *selected_path, uint32_t max_path_len);

int aegis_ui_set_theme(const char *theme_name);
int aegis_ui_get_theme(char *theme_name, uint32_t max_len);

int aegis_ui_set_dpi_scale(float scale);
float aegis_ui_get_dpi_scale(void);

#endif
