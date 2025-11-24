#include <devapi/ui_api.h>
#include <string.h>

int aegis_ui_init(void) { return 0; }
int aegis_ui_shutdown(void) { return 0; }
int aegis_ui_run_event_loop(void) { return 0; }
int aegis_ui_quit_event_loop(void) { return 0; }

aegis_ui_window_t *aegis_ui_window_create(const char *title, uint32_t width, uint32_t height, uint32_t style) { return 0; }
int aegis_ui_window_destroy(aegis_ui_window_t *window) { return 0; }
int aegis_ui_window_show(aegis_ui_window_t *window) { return 0; }
int aegis_ui_window_hide(aegis_ui_window_t *window) { return 0; }
int aegis_ui_window_set_size(aegis_ui_window_t *window, uint32_t width, uint32_t height) { return 0; }
int aegis_ui_window_get_size(aegis_ui_window_t *window, uint32_t *width, uint32_t *height) { return 0; }
int aegis_ui_window_set_title(aegis_ui_window_t *window, const char *title) { return 0; }
int aegis_ui_window_focus(aegis_ui_window_t *window) { return 0; }
int aegis_ui_window_is_focused(aegis_ui_window_t *window) { return 0; }

aegis_ui_component_t *aegis_ui_component_create(aegis_ui_window_t *window, 
                                                 aegis_ui_component_type_t type) { return 0; }
int aegis_ui_component_destroy(aegis_ui_component_t *comp) { return 0; }
int aegis_ui_component_set_position(aegis_ui_component_t *comp, int32_t x, int32_t y) { return 0; }
int aegis_ui_component_set_size(aegis_ui_component_t *comp, uint32_t width, uint32_t height) { return 0; }
int aegis_ui_component_get_position(aegis_ui_component_t *comp, int32_t *x, int32_t *y) { return 0; }
int aegis_ui_component_get_size(aegis_ui_component_t *comp, uint32_t *width, uint32_t *height) { return 0; }
int aegis_ui_component_set_visible(aegis_ui_component_t *comp, uint8_t visible) { return 0; }
int aegis_ui_component_get_visible(aegis_ui_component_t *comp) { return 0; }
int aegis_ui_component_set_enabled(aegis_ui_component_t *comp, uint8_t enabled) { return 0; }
int aegis_ui_component_get_enabled(aegis_ui_component_t *comp) { return 0; }

int aegis_ui_component_set_event_handler(aegis_ui_component_t *comp, 
                                         aegis_ui_event_handler_fn handler, void *ctx) { return 0; }
int aegis_ui_component_post_event(aegis_ui_component_t *comp, uint32_t event_type, void *event_data) { return 0; }

int aegis_ui_button_set_label(aegis_ui_component_t *button, const char *label) { return 0; }
int aegis_ui_button_get_label(aegis_ui_component_t *button, char *label, uint32_t max_len) { return 0; }

int aegis_ui_label_set_text(aegis_ui_component_t *label, const char *text) { return 0; }
int aegis_ui_label_get_text(aegis_ui_component_t *label, char *text, uint32_t max_len) { return 0; }
int aegis_ui_label_set_font(aegis_ui_component_t *label, const aegis_ui_font_t *font) { return 0; }

int aegis_ui_text_input_set_text(aegis_ui_component_t *input, const char *text) { return 0; }
int aegis_ui_text_input_get_text(aegis_ui_component_t *input, char *text, uint32_t max_len) { return 0; }
int aegis_ui_text_input_set_placeholder(aegis_ui_component_t *input, const char *placeholder) { return 0; }
int aegis_ui_text_input_set_password_mode(aegis_ui_component_t *input, uint8_t enabled) { return 0; }

int aegis_ui_checkbox_set_checked(aegis_ui_component_t *checkbox, uint8_t checked) { return 0; }
int aegis_ui_checkbox_get_checked(aegis_ui_component_t *checkbox) { return 0; }

int aegis_ui_progress_bar_set_value(aegis_ui_component_t *bar, uint32_t value) { return 0; }
int aegis_ui_progress_bar_set_max(aegis_ui_component_t *bar, uint32_t max) { return 0; }
int aegis_ui_progress_bar_get_value(aegis_ui_component_t *bar) { return 0; }

int aegis_ui_slider_set_value(aegis_ui_component_t *slider, int32_t value) { return 0; }
int aegis_ui_slider_set_range(aegis_ui_component_t *slider, int32_t min, int32_t max) { return 0; }
int aegis_ui_slider_get_value(aegis_ui_component_t *slider) { return 0; }

int aegis_ui_image_set_source(aegis_ui_component_t *image, const char *file_path) { return 0; }
int aegis_ui_image_set_data(aegis_ui_component_t *image, const uint8_t *data, 
                            uint32_t width, uint32_t height, uint8_t format) { return 0; }

int aegis_ui_canvas_clear(aegis_ui_component_t *canvas, aegis_ui_color_t color) { return 0; }
int aegis_ui_canvas_draw_line(aegis_ui_component_t *canvas, int32_t x1, int32_t y1, 
                              int32_t x2, int32_t y2, aegis_ui_color_t color, uint8_t thickness) { return 0; }
int aegis_ui_canvas_draw_rect(aegis_ui_component_t *canvas, const aegis_ui_rect_t *rect, 
                              aegis_ui_color_t color, uint8_t filled) { return 0; }
int aegis_ui_canvas_draw_circle(aegis_ui_component_t *canvas, int32_t cx, int32_t cy, 
                                uint32_t radius, aegis_ui_color_t color, uint8_t filled) { return 0; }
int aegis_ui_canvas_draw_text(aegis_ui_component_t *canvas, const char *text, 
                              int32_t x, int32_t y, aegis_ui_color_t color, const aegis_ui_font_t *font) { return 0; }

int aegis_ui_animate_property(aegis_ui_component_t *comp, const char *property, 
                              void *target_value, uint32_t duration_ms, 
                              aegis_ui_animation_fn callback, void *ctx) { return 0; }
int aegis_ui_animation_cancel(aegis_ui_component_t *comp) { return 0; }

int aegis_ui_show_dialog(aegis_ui_window_t *parent, const char *title, const char *message, 
                         uint32_t dialog_type, int *result) { return 0; }
int aegis_ui_show_file_dialog(aegis_ui_window_t *parent, const char *title, uint32_t mode, 
                              char *selected_path, uint32_t max_path_len) { return 0; }

int aegis_ui_set_theme(const char *theme_name) { return 0; }
int aegis_ui_get_theme(char *theme_name, uint32_t max_len) { return 0; }

int aegis_ui_set_dpi_scale(float scale) { return 0; }
float aegis_ui_get_dpi_scale(void) { return 1.0f; }
