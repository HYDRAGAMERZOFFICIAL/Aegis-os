#include <userland/aegis_ui.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    window_t *windows;
    u32 window_count;
    taskbar_item_t *taskbar_items;
    u32 taskbar_count;
    display_t display;
    ui_layout_t current_layout;
    theme_t current_theme;
    u32 max_windows;
    u32 max_taskbar_items;
    char current_wallpaper[256];
    char last_notification_title[128];
    char last_notification_msg[256];
    u32 notification_count;
} ui_state_t;

static ui_state_t ui_state = {0};
static int ui_initialized = 0;

int aegis_ui_init(void)
{
    if (ui_initialized) return 0;

    ui_state.max_windows = 512;
    ui_state.max_taskbar_items = 128;
    ui_state.window_count = 0;
    ui_state.taskbar_count = 0;
    ui_state.current_layout = LAYOUT_HYBRID;
    ui_state.current_theme = THEME_AUTO;
    ui_state.notification_count = 0;

    ui_state.windows = (window_t *)calloc(ui_state.max_windows, sizeof(window_t));
    if (!ui_state.windows) return -1;

    ui_state.taskbar_items = (taskbar_item_t *)calloc(ui_state.max_taskbar_items, sizeof(taskbar_item_t));
    if (!ui_state.taskbar_items) return -1;

    ui_state.display.x = 0;
    ui_state.display.y = 0;
    ui_state.display.width = 1920;
    ui_state.display.height = 1080;
    ui_state.display.background_pixels = (u64 *)calloc(1920 * 1080, sizeof(u64));

    strncpy(ui_state.current_wallpaper, "/system/media/wallpapers/aegis_quantum.png", 255);

    // Register default system taskbar items
    aegis_ui_update_taskbar_item(1, "Command Palette");
    aegis_ui_update_taskbar_item(2, "Aegis Terminal");
    aegis_ui_update_taskbar_item(3, "Neural Monitor");
    aegis_ui_update_taskbar_item(4, "Aegis Shield");
    aegis_ui_update_taskbar_item(5, "App Store");
    aegis_ui_update_taskbar_item(6, "Settings Center");

    ui_initialized = 1;
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
    if (!ui_initialized) aegis_ui_init();
    if (!title) return NULL;
    if (ui_state.window_count >= ui_state.max_windows) return NULL;

    window_t *window = &ui_state.windows[ui_state.window_count];
    window->window_id = ui_state.window_count + 1;
    window->title = title;
    window->x = 50 + (ui_state.window_count * 30) % 400;
    window->y = 50 + (ui_state.window_count * 30) % 300;
    window->width = (width > 0) ? width : 800;
    window->height = (height > 0) ? height : 600;
    window->focused = true;
    window->minimized = false;

    // Reset previous focus
    for (u32 i = 0; i < ui_state.window_count; i++) {
        ui_state.windows[i].focused = false;
    }

    ui_state.window_count++;
    return window;
}

int aegis_ui_destroy_window(u64 window_id)
{
    if (!ui_initialized || window_id == 0) return -1;

    for (u32 i = 0; i < ui_state.window_count; i++) {
        if (ui_state.windows[i].window_id == window_id) {
            for (u32 j = i; j < ui_state.window_count - 1; j++) {
                ui_state.windows[j] = ui_state.windows[j + 1];
            }
            ui_state.window_count--;
            if (ui_state.window_count > 0) {
                ui_state.windows[ui_state.window_count - 1].focused = true;
            }
            return 0;
        }
    }
    return -1;
}

int aegis_ui_set_window_focus(u64 window_id)
{
    if (!ui_initialized || window_id == 0) return -1;
    int found = 0;
    for (u32 i = 0; i < ui_state.window_count; i++) {
        if (ui_state.windows[i].window_id == window_id) {
            ui_state.windows[i].focused = true;
            ui_state.windows[i].minimized = false;
            found = 1;
        } else {
            ui_state.windows[i].focused = false;
        }
    }
    return found ? 0 : -1;
}

int aegis_ui_minimize_window(u64 window_id)
{
    if (!ui_initialized || window_id == 0) return -1;
    for (u32 i = 0; i < ui_state.window_count; i++) {
        if (ui_state.windows[i].window_id == window_id) {
            ui_state.windows[i].minimized = true;
            ui_state.windows[i].focused = false;
            return 0;
        }
    }
    return -1;
}

int aegis_ui_maximize_window(u64 window_id)
{
    if (!ui_initialized || window_id == 0) return -1;
    for (u32 i = 0; i < ui_state.window_count; i++) {
        if (ui_state.windows[i].window_id == window_id) {
            ui_state.windows[i].minimized = false;
            ui_state.windows[i].x = 0;
            ui_state.windows[i].y = 0;
            ui_state.windows[i].width = ui_state.display.width;
            ui_state.windows[i].height = ui_state.display.height;
            ui_state.windows[i].focused = true;
            return 0;
        }
    }
    return -1;
}

int aegis_ui_render_taskbar(void)
{
    return 0;
}

int aegis_ui_update_taskbar_item(u64 app_id, const char *app_name)
{
    if (!app_name) return -1;
    for (u32 i = 0; i < ui_state.taskbar_count; i++) {
        if (ui_state.taskbar_items[i].app_id == app_id) {
            ui_state.taskbar_items[i].app_name = app_name;
            return 0;
        }
    }
    if (ui_state.taskbar_count >= ui_state.max_taskbar_items) return -1;

    taskbar_item_t *item = &ui_state.taskbar_items[ui_state.taskbar_count++];
    item->app_id = app_id;
    item->app_name = app_name;
    item->app_icon = "default_icon";
    item->category = "System";
    item->pinned = true;
    return 0;
}

int aegis_ui_show_notification(const char *title, const char *message, u32 duration)
{
    if (!title || !message) return -1;
    (void)duration;
    strncpy(ui_state.last_notification_title, title, 127);
    strncpy(ui_state.last_notification_msg, message, 255);
    ui_state.notification_count++;
    return 0;
}

int aegis_ui_set_wallpaper(const char *wallpaper_path)
{
    if (!wallpaper_path) return -1;
    strncpy(ui_state.current_wallpaper, wallpaper_path, 255);
    return 0;
}

display_t *aegis_ui_get_display_info(void)
{
    if (!ui_initialized) aegis_ui_init();
    return &ui_state.display;
}

int aegis_ui_handle_input(u32 key_code, u64 window_id)
{
    (void)key_code;
    return aegis_ui_set_window_focus(window_id);
}

int aegis_ui_render_frame(void)
{
    return 0;
}
