#include <userland/aegis_ui_theme.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PRESET_THEMES 8

static ui_mode_t g_current_mode = UI_MODE_PRODUCTIVITY;
static screen_size_t g_current_screen_size = SCREEN_SIZE_DESKTOP;
static ui_theme_t g_current_theme = {0};
static ui_theme_t g_preset_themes[MAX_PRESET_THEMES] = {0};
static uint32_t g_preset_count = 0;
static uint8_t g_adaptive_enabled = 1;
static uint8_t g_user_focus_level = 80;
static uint8_t g_is_multitasking = 0;
static float g_dpi_scale = 1.0f;

static void init_preset_themes(void) {
    g_preset_count = 0;

    // Theme 1: Aegis Quantum Cyan (Default Productivity)
    strncpy(g_preset_themes[0].name, "Quantum Cyan", 63);
    strncpy(g_preset_themes[0].primary_color, "#00e5ff", 6);
    strncpy(g_preset_themes[0].secondary_color, "#0077b6", 6);
    strncpy(g_preset_themes[0].accent_color, "#7209b7", 6);
    strncpy(g_preset_themes[0].text_color, "#f8f9fa", 6);
    strncpy(g_preset_themes[0].background_color, "#0a0e17", 6);
    g_preset_count++;

    // Theme 2: Cyberpunk Amber (Gaming)
    strncpy(g_preset_themes[1].name, "Cyberpunk Amber", 63);
    strncpy(g_preset_themes[1].primary_color, "#ffb703", 6);
    strncpy(g_preset_themes[1].secondary_color, "#fb8500", 6);
    strncpy(g_preset_themes[1].accent_color, "#f72585", 6);
    strncpy(g_preset_themes[1].text_color, "#ffffff", 6);
    strncpy(g_preset_themes[1].background_color, "#121214", 6);
    g_preset_count++;

    // Theme 3: Deep Space Dark
    strncpy(g_preset_themes[2].name, "Deep Space Dark", 63);
    strncpy(g_preset_themes[2].primary_color, "#64dfdf", 6);
    strncpy(g_preset_themes[2].secondary_color, "#48bfe3", 6);
    strncpy(g_preset_themes[2].accent_color, "#5390d9", 6);
    strncpy(g_preset_themes[2].text_color, "#e2e8f0", 6);
    strncpy(g_preset_themes[2].background_color, "#05070c", 6);
    g_preset_count++;

    // Theme 4: Crystal Light
    strncpy(g_preset_themes[3].name, "Crystal Light", 63);
    strncpy(g_preset_themes[3].primary_color, "#0077b6", 6);
    strncpy(g_preset_themes[3].secondary_color, "#0096c7", 6);
    strncpy(g_preset_themes[3].accent_color, "#03045e", 6);
    strncpy(g_preset_themes[3].text_color, "#1e293b", 6);
    strncpy(g_preset_themes[3].background_color, "#f8fafc", 6);
    g_preset_count++;
}

int ui_theme_init(void) {
    init_preset_themes();
    g_current_theme = g_preset_themes[0];
    g_current_mode = UI_MODE_PRODUCTIVITY;
    g_current_screen_size = SCREEN_SIZE_DESKTOP;
    g_adaptive_enabled = 1;
    g_dpi_scale = 1.0f;
    return 0;
}

int ui_theme_set_mode(ui_mode_t mode) {
    if (mode < UI_MODE_PRODUCTIVITY || mode > UI_MODE_LIGHT) return -1;
    g_current_mode = mode;

    switch (mode) {
        case UI_MODE_GAMING:
            g_current_theme = g_preset_themes[1];
            break;
        case UI_MODE_DARK:
        case UI_MODE_MINIMAL:
            g_current_theme = g_preset_themes[2];
            break;
        case UI_MODE_LIGHT:
            g_current_theme = g_preset_themes[3];
            break;
        case UI_MODE_PRODUCTIVITY:
        default:
            g_current_theme = g_preset_themes[0];
            break;
    }
    return 0;
}

int ui_theme_get_mode(ui_mode_t *mode) {
    if (!mode) return -1;
    *mode = g_current_mode;
    return 0;
}

int ui_theme_detect_screen_size(screen_size_t *size) {
    if (!size) return -1;
    *size = g_current_screen_size;
    return 0;
}

int ui_theme_apply_for_screen(screen_size_t screen_size) {
    g_current_screen_size = screen_size;
    switch (screen_size) {
        case SCREEN_SIZE_MOBILE:
            g_dpi_scale = 0.8f;
            break;
        case SCREEN_SIZE_TABLET:
            g_dpi_scale = 0.9f;
            break;
        case SCREEN_SIZE_DESKTOP:
            g_dpi_scale = 1.0f;
            break;
        case SCREEN_SIZE_ULTRAWIDE:
            g_dpi_scale = 1.25f;
            break;
        default:
            g_dpi_scale = 1.0f;
            break;
    }
    return 0;
}

int ui_theme_set_custom_theme(const ui_theme_t *theme) {
    if (!theme) return -1;
    g_current_theme = *theme;
    return 0;
}

int ui_theme_get_current_theme(ui_theme_t *theme) {
    if (!theme) return -1;
    *theme = g_current_theme;
    return 0;
}

int ui_theme_detect_user_focus(uint8_t *focus_level) {
    if (!focus_level) return -1;
    *focus_level = g_user_focus_level;
    return 0;
}

int ui_theme_detect_multitasking_mode(uint8_t *is_multitasking) {
    if (!is_multitasking) return -1;
    *is_multitasking = g_is_multitasking;
    return 0;
}

int ui_theme_auto_adjust(void) {
    if (!g_adaptive_enabled) return -1;
    if (g_is_multitasking) {
        g_dpi_scale = 0.95f;
    } else {
        g_dpi_scale = 1.0f;
    }
    return 0;
}

int ui_theme_enable_adaptive_ui(void) {
    g_adaptive_enabled = 1;
    return 0;
}

int ui_theme_disable_adaptive_ui(void) {
    g_adaptive_enabled = 0;
    return 0;
}

int ui_theme_get_available_themes(ui_theme_t *themes, uint32_t *count, uint32_t max_themes) {
    if (!themes || !count) return -1;
    uint32_t to_copy = (g_preset_count < max_themes) ? g_preset_count : max_themes;
    for (uint32_t i = 0; i < to_copy; i++) {
        themes[i] = g_preset_themes[i];
    }
    *count = to_copy;
    return 0;
}

int ui_theme_import_custom_theme(const char *theme_file) {
    if (!theme_file) return -1;
    // Mock import logic
    strncpy(g_current_theme.name, "Imported Theme", 63);
    strncpy(g_current_theme.primary_color, "#8a2be2", 6);
    strncpy(g_current_theme.secondary_color, "#4a0e4e", 6);
    strncpy(g_current_theme.accent_color, "#00ffcc", 6);
    strncpy(g_current_theme.text_color, "#ffffff", 6);
    strncpy(g_current_theme.background_color, "#100820", 6);
    return 0;
}

float ui_theme_get_dpi(void) {
    return g_dpi_scale;
}

int ui_theme_set_dpi_scale(float scale) {
    if (scale < 0.5f || scale > 3.0f) return -1;
    g_dpi_scale = scale;
    return 0;
}
