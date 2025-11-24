#ifndef AEGIS_UI_THEME_H
#define AEGIS_UI_THEME_H

#include <kernel/types.h>

typedef enum {
    UI_MODE_PRODUCTIVITY = 1,
    UI_MODE_TABLET = 2,
    UI_MODE_GAMING = 3,
    UI_MODE_MINIMAL = 4,
    UI_MODE_DARK = 5,
    UI_MODE_LIGHT = 6,
} ui_mode_t;

typedef enum {
    SCREEN_SIZE_MOBILE = 1,
    SCREEN_SIZE_TABLET = 2,
    SCREEN_SIZE_DESKTOP = 3,
    SCREEN_SIZE_ULTRAWIDE = 4,
} screen_size_t;

typedef struct {
    char name[64];
    char primary_color[7];
    char secondary_color[7];
    char accent_color[7];
    char text_color[7];
    char background_color[7];
} ui_theme_t;

int ui_theme_init(void);
int ui_theme_set_mode(ui_mode_t mode);
int ui_theme_get_mode(ui_mode_t *mode);

int ui_theme_detect_screen_size(screen_size_t *size);
int ui_theme_apply_for_screen(screen_size_t screen_size);

int ui_theme_set_custom_theme(const ui_theme_t *theme);
int ui_theme_get_current_theme(ui_theme_t *theme);

int ui_theme_detect_user_focus(uint8_t *focus_level);
int ui_theme_detect_multitasking_mode(uint8_t *is_multitasking);

int ui_theme_auto_adjust(void);
int ui_theme_enable_adaptive_ui(void);
int ui_theme_disable_adaptive_ui(void);

int ui_theme_get_available_themes(ui_theme_t *themes, uint32_t *count, uint32_t max_themes);
int ui_theme_import_custom_theme(const char *theme_file);

float ui_theme_get_dpi(void);
int ui_theme_set_dpi_scale(float scale);

#endif
