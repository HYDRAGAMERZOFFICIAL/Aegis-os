#ifndef AEGIS_SERVICES_COMMAND_PALETTE_H
#define AEGIS_SERVICES_COMMAND_PALETTE_H

#include <kernel/types.h>

typedef enum {
    CMD_CATEGORY_APPS = 1,
    CMD_CATEGORY_SETTINGS = 2,
    CMD_CATEGORY_FILES = 3,
    CMD_CATEGORY_SYSTEM = 4,
    CMD_CATEGORY_HELP = 5,
} cmd_category_t;

typedef struct {
    char name[128];
    char description[256];
    char icon_path[256];
    cmd_category_t category;
    float relevance_score;
    void (*execute_fn)(void);
} command_entry_t;

int cmd_palette_init(void);
int cmd_palette_show(void);
int cmd_palette_hide(void);
int cmd_palette_is_visible(void);

int cmd_palette_search(const char *query, command_entry_t *results, 
                       uint32_t *count, uint32_t max_results);
int cmd_palette_execute_command(const char *command_name);

int cmd_palette_register_command(const command_entry_t *command);
int cmd_palette_unregister_command(const char *command_name);

int cmd_palette_get_recent_commands(char *commands, uint32_t max_len);
int cmd_palette_clear_history(void);

int cmd_palette_search_apps(const char *query, char *results, uint32_t max_len);
int cmd_palette_search_settings(const char *query, char *results, uint32_t max_len);
int cmd_palette_search_files(const char *query, char *results, uint32_t max_len);

int cmd_palette_enable_fuzzy_search(void);
int cmd_palette_set_max_results(uint32_t max_results);

#endif
