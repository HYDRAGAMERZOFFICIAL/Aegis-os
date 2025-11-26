#include <userland/aegis_ui.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    window_t *windows;
    u32 window_count;
    taskbar_item_t *taskbar_items;
    u32 taskbar_count;
    display_t *display;
    ui_layout_t current_layout;
    theme_t current_theme;
    u32 max_windows;
    u32 max_taskbar_items;
} ui_state_t;

static ui_state_t ui_state = {0};

int aegis_ui_init(void)
{
    ui_state.max_windows = 512;
    ui_state.max_taskbar_items = 128;
    ui_state.window_count = 0;
    ui_state.taskbar_count = 0;
    ui_state.current_layout = LAYOUT_HYBRID;
    ui_state.current_theme = THEME_AUTO;

    ui_state.windows = (window_t *)calloc(ui_state.max_windows, sizeof(window_t));
    if (!ui_state.windows) return -1;

    ui_state.taskbar_items = (taskbar_item_t *)calloc(ui_state.max_taskbar_items, sizeof(taskbar_item_t));
    if (!ui_state.taskbar_items) return -1;

    ui_state.display = (display_t *)calloc(1, sizeof(display_t));
    if (!ui_state.display) return -1;

    ui_state.display->width = 1920;
    ui_state.display->height = 1080;
    ui_state.display->background_pixels = (u64 *)calloc(ui_state.display->width * ui_state.display->height, sizeof(u64));

    return 0;
}

int aegis_ui_set_layout(ui_layout_t layout)
{
    ui_state.current_layout = layout;
    return 0;
}

int aegis_ui_set_theme(theme_t theme)
{
    ui_state.current_theme = theme;
    return 0;
}

window_t *aegis_ui_create_window(const char *title, u32 width, u32 height)
{
    if (!title) return NULL;

    if (ui_state.window_count >= ui_state.max_windows) return NULL;

    window_t *window = &ui_state.windows[ui_state.window_count];
    window->window_id = ui_state.window_count;
    window->title = title;
    window->x = 0;
    window->y = 0;
    window->width = width;
    window->height = height;
    window->focused = 0;
    window->minimized = 0;

    ui_state.window_count++;

    return window;
}

int aegis_ui_destroy_window(u64 window_id)
{
    if (window_id >= ui_state.window_count) return -1;

    ui_state.windows[window_id] = ui_state.windows[ui_state.window_count - 1];
    ui_state.window_count--;

    return 0;
}

int aegis_ui_set_window_focus(u64 window_id)
{
    if (window_id >= ui_state.window_count) return -1;

    for (u32 i = 0; i < ui_state.window_count; i++) {
        ui_state.windows[i].focused = 0;
    }

    ui_state.windows[window_id].focused = 1;

    return 0;
}

int aegis_ui_minimize_window(u64 window_id)
{
    if (window_id >= ui_state.window_count) return -1;

    ui_state.windows[window_id].minimized = 1;

    return 0;
}

int aegis_ui_maximize_window(u64 window_id)
{
    if (window_id >= ui_state.window_count) return -1;

    ui_state.windows[window_id].minimized = 0;
    ui_state.windows[window_id].width = ui_state.display->width;
    ui_state.windows[window_id].height = ui_state.display->height;

    return 0;
}

int aegis_ui_render_taskbar(void)
{
    return 0;
}

int aegis_ui_update_taskbar_item(u64 app_id, const char *app_name)
{
    if (!app_name) return -1;

    if (ui_state.taskbar_count >= ui_state.max_taskbar_items) return -1;

    taskbar_item_t *item = &ui_state.taskbar_items[ui_state.taskbar_count];
    item->app_id = app_id;
    item->app_name = app_name;
    item->pinned = 0;

    ui_state.taskbar_count++;

    return 0;
}

int aegis_ui_show_notification(const char *title, const char *message, u32 duration)
{
    if (!title || !message) return -1;

    return 0;
}

int aegis_ui_set_wallpaper(const char *wallpaper_path)
{
    if (!wallpaper_path) return -1;

    return 0;
}

display_t *aegis_ui_get_display_info(void)
{
    return ui_state.display;
}

int aegis_ui_handle_input(u32 key_code, u64 window_id)
{
    if (window_id >= ui_state.window_count) return -1;

    return 0;
}

int aegis_ui_render_frame(void)
{
    return 0;
}
