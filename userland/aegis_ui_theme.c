#include <userland/aegis_ui_theme.h>
#include <string.h>

int ui_theme_init(void) { return 0; }
int ui_theme_set_mode(ui_mode_t mode) { return 0; }
int ui_theme_get_mode(ui_mode_t *mode) { return 0; }
int ui_theme_detect_screen_size(screen_size_t *size) { return 0; }
int ui_theme_apply_for_screen(screen_size_t screen_size) { return 0; }
int ui_theme_set_custom_theme(const ui_theme_t *theme) { return 0; }
int ui_theme_get_current_theme(ui_theme_t *theme) { return 0; }
int ui_theme_detect_user_focus(uint8_t *focus_level) { return 0; }
int ui_theme_detect_multitasking_mode(uint8_t *is_multitasking) { return 0; }
int ui_theme_auto_adjust(void) { return 0; }
int ui_theme_enable_adaptive_ui(void) { return 0; }
int ui_theme_disable_adaptive_ui(void) { return 0; }
int ui_theme_get_available_themes(ui_theme_t *themes, uint32_t *count, uint32_t max_themes) { return 0; }
int ui_theme_import_custom_theme(const char *theme_file) { return 0; }
float ui_theme_get_dpi(void) { return 1.0f; }
int ui_theme_set_dpi_scale(float scale) { return 0; }
