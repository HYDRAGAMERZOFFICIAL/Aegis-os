#ifndef AEGIS_UI_H
#define AEGIS_UI_H

#include <kernel/types.h>

typedef enum {
    LAYOUT_WINDOWS,
    LAYOUT_LINUX,
    LAYOUT_HYBRID
} ui_layout_t;

typedef enum {
    THEME_LIGHT,
    THEME_DARK,
    THEME_AUTO
} theme_t;

typedef struct {
    u64 window_id;
    const char *title;
    u32 x, y, width, height;
    bool focused;
    bool minimized;
} window_t;

typedef struct {
    u64 app_id;
    const char *app_name;
    const char *app_icon;
    const char *category;
    bool pinned;
} taskbar_item_t;

typedef struct {
    u32 x, y;
    u32 width, height;
    u64 *background_pixels;
} display_t;

int aegis_ui_init(void);
int aegis_ui_set_layout(ui_layout_t layout);
int aegis_ui_set_theme(theme_t theme);
window_t *aegis_ui_create_window(const char *title, u32 width, u32 height);
int aegis_ui_destroy_window(u64 window_id);
int aegis_ui_set_window_focus(u64 window_id);
int aegis_ui_minimize_window(u64 window_id);
int aegis_ui_maximize_window(u64 window_id);
int aegis_ui_render_taskbar(void);
int aegis_ui_update_taskbar_item(u64 app_id, const char *app_name);
int aegis_ui_show_notification(const char *title, const char *message, u32 duration);
int aegis_ui_set_wallpaper(const char *wallpaper_path);
display_t *aegis_ui_get_display_info(void);
int aegis_ui_handle_input(u32 key_code, u64 window_id);
int aegis_ui_render_frame(void);

#endif
