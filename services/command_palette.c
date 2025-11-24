#include <services/command_palette.h>
#include <string.h>

int cmd_palette_init(void) { return 0; }
int cmd_palette_show(void) { return 0; }
int cmd_palette_hide(void) { return 0; }
int cmd_palette_is_visible(void) { return 0; }
int cmd_palette_search(const char *query, command_entry_t *results, uint32_t *count, uint32_t max_results) { return 0; }
int cmd_palette_execute_command(const char *command_name) { return 0; }
int cmd_palette_register_command(const command_entry_t *command) { return 0; }
int cmd_palette_unregister_command(const char *command_name) { return 0; }
int cmd_palette_get_recent_commands(char *commands, uint32_t max_len) { return 0; }
int cmd_palette_clear_history(void) { return 0; }
int cmd_palette_search_apps(const char *query, char *results, uint32_t max_len) { return 0; }
int cmd_palette_search_settings(const char *query, char *results, uint32_t max_len) { return 0; }
int cmd_palette_search_files(const char *query, char *results, uint32_t max_len) { return 0; }
int cmd_palette_enable_fuzzy_search(void) { return 0; }
int cmd_palette_set_max_results(uint32_t max_results) { return 0; }
