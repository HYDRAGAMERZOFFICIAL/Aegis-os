#ifndef AEGIS_SETTINGS_CENTER_H
#define AEGIS_SETTINGS_CENTER_H

#include <kernel/types.h>

typedef enum {
    SETTING_CATEGORY_DISPLAY,
    SETTING_CATEGORY_SOUND,
    SETTING_CATEGORY_NETWORK,
    SETTING_CATEGORY_SECURITY,
    SETTING_CATEGORY_STORAGE,
    SETTING_CATEGORY_SYSTEM,
    SETTING_CATEGORY_PRIVACY,
    SETTING_CATEGORY_ABOUT
} setting_category_t;

typedef struct {
    const char *setting_name;
    const char *category;
    const char *value;
    const char *description;
    bool is_user_facing;
} setting_t;

typedef struct {
    u32 brightness;
    u32 refresh_rate;
    const char *resolution;
    bool night_light;
} display_settings_t;

typedef struct {
    u32 volume;
    bool mute;
    const char *output_device;
} audio_settings_t;

int settings_init(void);
int settings_get(const char *setting_name, char *value_buf, u64 buf_size);
int settings_set(const char *setting_name, const char *value);
int settings_get_category(setting_category_t cat, setting_t **settings, u32 *count);
int settings_apply_display_settings(display_settings_t *settings);
int settings_apply_audio_settings(audio_settings_t *settings);
int settings_reset_to_defaults(void);
int settings_export_settings(const char *filename);
int settings_import_settings(const char *filename);
int settings_enable_developer_mode(void);
int settings_disable_developer_mode(void);
int settings_get_system_info(char *info_buf, u64 buf_size);
int settings_check_updates(void);

#endif
