#include <userland/settings_center.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    setting_t *settings;
    u32 setting_count;
    display_settings_t display_settings;
    audio_settings_t audio_settings;
    u32 max_settings;
    u32 developer_mode_enabled;
} settings_state_t;

static settings_state_t settings_state = {0};

int settings_init(void)
{
    settings_state.max_settings = 1024;
    settings_state.setting_count = 0;
    settings_state.developer_mode_enabled = 0;

    settings_state.settings = (setting_t *)calloc(settings_state.max_settings, sizeof(setting_t));
    if (!settings_state.settings) return -1;

    settings_state.display_settings.brightness = 100;
    settings_state.display_settings.refresh_rate = 60;
    settings_state.display_settings.resolution = "1920x1080";
    settings_state.display_settings.night_light = 0;

    settings_state.audio_settings.volume = 100;
    settings_state.audio_settings.mute = 0;
    settings_state.audio_settings.output_device = "default";

    return 0;
}

int settings_get(const char *setting_name, char *value_buf, u64 buf_size)
{
    if (!setting_name || !value_buf || buf_size == 0) return -1;

    for (u32 i = 0; i < settings_state.setting_count; i++) {
        if (strcmp(settings_state.settings[i].setting_name, setting_name) == 0) {
            strncpy(value_buf, settings_state.settings[i].value, buf_size - 1);
            value_buf[buf_size - 1] = '\0';
            return 0;
        }
    }

    return -1;
}

int settings_set(const char *setting_name, const char *value)
{
    if (!setting_name || !value) return -1;

    for (u32 i = 0; i < settings_state.setting_count; i++) {
        if (strcmp(settings_state.settings[i].setting_name, setting_name) == 0) {
            settings_state.settings[i].value = value;
            return 0;
        }
    }

    if (settings_state.setting_count >= settings_state.max_settings) return -1;

    setting_t *setting = &settings_state.settings[settings_state.setting_count];
    setting->setting_name = setting_name;
    setting->value = value;
    setting->category = "general";
    setting->description = "";
    setting->is_user_facing = 1;

    settings_state.setting_count++;

    return 0;
}

int settings_get_category(setting_category_t cat, setting_t **settings, u32 *count)
{
    if (!settings || !count) return -1;

    setting_t **result = (setting_t **)calloc(settings_state.setting_count, sizeof(setting_t *));
    if (!result) return -1;

    u32 result_count = 0;
    for (u32 i = 0; i < settings_state.setting_count; i++) {
        result[result_count++] = &settings_state.settings[i];
    }

    *settings = (setting_t *)result;
    *count = result_count;

    return 0;
}

int settings_apply_display_settings(display_settings_t *settings)
{
    if (!settings) return -1;

    memcpy(&settings_state.display_settings, settings, sizeof(display_settings_t));

    return 0;
}

int settings_apply_audio_settings(audio_settings_t *settings)
{
    if (!settings) return -1;

    memcpy(&settings_state.audio_settings, settings, sizeof(audio_settings_t));

    return 0;
}

int settings_reset_to_defaults(void)
{
    settings_state.setting_count = 0;
    settings_state.developer_mode_enabled = 0;
    settings_state.display_settings.brightness = 100;
    settings_state.display_settings.refresh_rate = 60;
    settings_state.audio_settings.volume = 100;
    settings_state.audio_settings.mute = 0;

    return 0;
}

int settings_export_settings(const char *filename)
{
    if (!filename) return -1;

    return 0;
}

int settings_import_settings(const char *filename)
{
    if (!filename) return -1;

    return 0;
}

int settings_enable_developer_mode(void)
{
    settings_state.developer_mode_enabled = 1;
    return 0;
}

int settings_disable_developer_mode(void)
{
    settings_state.developer_mode_enabled = 0;
    return 0;
}

int settings_get_system_info(char *info_buf, u64 buf_size)
{
    if (!info_buf || buf_size == 0) return -1;

    strncpy(info_buf, "Aegis OS System Information", buf_size - 1);
    info_buf[buf_size - 1] = '\0';

    return 0;
}

int settings_check_updates(void)
{
    return 0;
}
