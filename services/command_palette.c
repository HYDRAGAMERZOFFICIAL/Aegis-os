#include <services/command_palette.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_COMMANDS 256
#define MAX_HISTORY 64

static command_entry_t g_commands[MAX_COMMANDS] = {0};
static uint32_t g_command_count = 0;
static char g_history[MAX_HISTORY][128] = {0};
static uint32_t g_history_count = 0;
static uint8_t g_is_visible = 0;
static uint8_t g_fuzzy_search_enabled = 1;
static uint32_t g_max_results = 20;

static void str_tolower(char *dst, const char *src, size_t max) {
    size_t i = 0;
    while (src[i] && i < max - 1) {
        dst[i] = (char)tolower((unsigned char)src[i]);
        i++;
    }
    dst[i] = '\0';
}

static float calc_relevance(const char *query, const char *target) {
    char q_lower[128] = {0};
    char t_lower[256] = {0};
    str_tolower(q_lower, query, sizeof(q_lower));
    str_tolower(t_lower, target, sizeof(t_lower));

    if (strcmp(q_lower, t_lower) == 0) return 1.0f;
    if (strstr(t_lower, q_lower) == t_lower) return 0.9f;
    if (strstr(t_lower, q_lower) != NULL) return 0.7f;

    if (g_fuzzy_search_enabled) {
        size_t qi = 0, ti = 0;
        size_t matches = 0;
        while (q_lower[qi] && t_lower[ti]) {
            if (q_lower[qi] == t_lower[ti]) {
                matches++;
                qi++;
            }
            ti++;
        }
        if (qi == strlen(q_lower) && strlen(q_lower) > 0) {
            return 0.5f + (float)matches / (float)strlen(t_lower) * 0.2f;
        }
    }
    return 0.0f;
}

int cmd_palette_init(void) {
    g_command_count = 0;
    g_history_count = 0;
    g_is_visible = 0;
    g_fuzzy_search_enabled = 1;
    g_max_results = 20;

    // Register built-in system commands
    command_entry_t c;
    
    memset(&c, 0, sizeof(c));
    strncpy(c.name, "Aegis Shell / Terminal", 127);
    strncpy(c.description, "Open the interactive Aegis OS command-line shell", 255);
    strncpy(c.icon_path, "terminal_icon", 255);
    c.category = CMD_CATEGORY_APPS;
    cmd_palette_register_command(&c);

    memset(&c, 0, sizeof(c));
    strncpy(c.name, "Neural Activity Monitor", 127);
    strncpy(c.description, "Real-time AI activity, kernel thread graph, and entropy monitor", 255);
    strncpy(c.icon_path, "neural_icon", 255);
    c.category = CMD_CATEGORY_SYSTEM;
    cmd_palette_register_command(&c);

    memset(&c, 0, sizeof(c));
    strncpy(c.name, "Aegis Shield Security Center", 127);
    strncpy(c.description, "Firewall rules, IDS/IPS status, and zero-trust verification", 255);
    strncpy(c.icon_path, "shield_icon", 255);
    c.category = CMD_CATEGORY_SYSTEM;
    cmd_palette_register_command(&c);

    memset(&c, 0, sizeof(c));
    strncpy(c.name, "System Troubleshooter & Auto-Fix", 127);
    strncpy(c.description, "One-tap diagnosis and remediation for system health", 255);
    strncpy(c.icon_path, "troubleshoot_icon", 255);
    c.category = CMD_CATEGORY_SYSTEM;
    cmd_palette_register_command(&c);

    memset(&c, 0, sizeof(c));
    strncpy(c.name, "Task Optimizer (Game / Work Mode)", 127);
    strncpy(c.description, "Dynamic system resource optimization and CPU/GPU boosting", 255);
    strncpy(c.icon_path, "optimizer_icon", 255);
    c.category = CMD_CATEGORY_SYSTEM;
    cmd_palette_register_command(&c);

    memset(&c, 0, sizeof(c));
    strncpy(c.name, "System Checkpoint & Rollback", 127);
    strncpy(c.description, "Create snapshots and perform instant 10s system rollback", 255);
    strncpy(c.icon_path, "checkpoint_icon", 255);
    c.category = CMD_CATEGORY_SYSTEM;
    cmd_palette_register_command(&c);

    memset(&c, 0, sizeof(c));
    strncpy(c.name, "Aegis App Store", 127);
    strncpy(c.description, "Discover and install sandboxed applications", 255);
    strncpy(c.icon_path, "store_icon", 255);
    c.category = CMD_CATEGORY_APPS;
    cmd_palette_register_command(&c);

    memset(&c, 0, sizeof(c));
    strncpy(c.name, "Settings & Theme Center", 127);
    strncpy(c.description, "Configure desktop layout, adaptive theme, and network", 255);
    strncpy(c.icon_path, "settings_icon", 255);
    c.category = CMD_CATEGORY_SETTINGS;
    cmd_palette_register_command(&c);

    return 0;
}

int cmd_palette_show(void) {
    g_is_visible = 1;
    return 0;
}

int cmd_palette_hide(void) {
    g_is_visible = 0;
    return 0;
}

int cmd_palette_is_visible(void) {
    return g_is_visible;
}

int cmd_palette_search(const char *query, command_entry_t *results, 
                       uint32_t *count, uint32_t max_results) {
    if (!results || !count) return -1;
    if (!query || strlen(query) == 0) {
        // Return top results up to limit
        uint32_t limit = (g_command_count < max_results) ? g_command_count : max_results;
        for (uint32_t i = 0; i < limit; i++) {
            results[i] = g_commands[i];
            results[i].relevance_score = 1.0f;
        }
        *count = limit;
        return 0;
    }

    uint32_t match_count = 0;
    for (uint32_t i = 0; i < g_command_count && match_count < max_results; i++) {
        float score_name = calc_relevance(query, g_commands[i].name);
        float score_desc = calc_relevance(query, g_commands[i].description);
        float score = (score_name > score_desc) ? score_name : score_desc;

        if (score > 0.0f) {
            results[match_count] = g_commands[i];
            results[match_count].relevance_score = score;
            match_count++;
        }
    }
    *count = match_count;
    return 0;
}

int cmd_palette_execute_command(const char *command_name) {
    if (!command_name) return -1;

    for (uint32_t i = 0; i < g_command_count; i++) {
        if (strcmp(g_commands[i].name, command_name) == 0) {
            if (g_history_count < MAX_HISTORY) {
                strncpy(g_history[g_history_count++], command_name, 127);
            }
            if (g_commands[i].execute_fn) {
                g_commands[i].execute_fn();
            }
            return 0;
        }
    }
    return -1;
}

int cmd_palette_register_command(const command_entry_t *command) {
    if (!command || g_command_count >= MAX_COMMANDS) return -1;
    g_commands[g_command_count++] = *command;
    return 0;
}

int cmd_palette_unregister_command(const char *command_name) {
    if (!command_name) return -1;
    for (uint32_t i = 0; i < g_command_count; i++) {
        if (strcmp(g_commands[i].name, command_name) == 0) {
            for (uint32_t j = i; j < g_command_count - 1; j++) {
                g_commands[j] = g_commands[j + 1];
            }
            g_command_count--;
            return 0;
        }
    }
    return -1;
}

int cmd_palette_get_recent_commands(char *commands, uint32_t max_len) {
    if (!commands || max_len == 0) return -1;
    commands[0] = '\0';
    for (uint32_t i = 0; i < g_history_count; i++) {
        strncat(commands, g_history[i], max_len - strlen(commands) - 1);
        if (i + 1 < g_history_count) {
            strncat(commands, "\n", max_len - strlen(commands) - 1);
        }
    }
    return 0;
}

int cmd_palette_clear_history(void) {
    g_history_count = 0;
    return 0;
}

int cmd_palette_search_apps(const char *query, char *results, uint32_t max_len) {
    if (!results || max_len == 0) return -1;
    results[0] = '\0';
    for (uint32_t i = 0; i < g_command_count; i++) {
        if (g_commands[i].category == CMD_CATEGORY_APPS) {
            if (!query || strlen(query) == 0 || calc_relevance(query, g_commands[i].name) > 0.0f) {
                strncat(results, g_commands[i].name, max_len - strlen(results) - 1);
                strncat(results, ";", max_len - strlen(results) - 1);
            }
        }
    }
    return 0;
}

int cmd_palette_search_settings(const char *query, char *results, uint32_t max_len) {
    if (!results || max_len == 0) return -1;
    results[0] = '\0';
    for (uint32_t i = 0; i < g_command_count; i++) {
        if (g_commands[i].category == CMD_CATEGORY_SETTINGS) {
            if (!query || strlen(query) == 0 || calc_relevance(query, g_commands[i].name) > 0.0f) {
                strncat(results, g_commands[i].name, max_len - strlen(results) - 1);
                strncat(results, ";", max_len - strlen(results) - 1);
            }
        }
    }
    return 0;
}

int cmd_palette_search_files(const char *query, char *results, uint32_t max_len) {
    if (!results || max_len == 0) return -1;
    // Simulated file search
    snprintf(results, max_len, "/home/user/document.txt;/system/config/aegis.conf;/media/images/wallpaper.png");
    (void)query;
    return 0;
}

int cmd_palette_enable_fuzzy_search(void) {
    g_fuzzy_search_enabled = 1;
    return 0;
}

int cmd_palette_set_max_results(uint32_t max_results) {
    g_max_results = max_results;
    return 0;
}
