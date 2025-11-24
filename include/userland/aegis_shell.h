#ifndef AEGIS_SHELL_H
#define AEGIS_SHELL_H

#include <kernel/types.h>

typedef enum {
    CMD_TYPE_BUILTIN,
    CMD_TYPE_EXTERNAL,
    CMD_TYPE_SCRIPT,
    CMD_TYPE_ALIAS
} command_type_t;

typedef struct {
    const char *command;
    const char *description;
    command_type_t type;
} command_info_t;

typedef struct {
    const char *name;
    const char *expansion;
} alias_t;

typedef struct {
    char *command_line;
    u64 start_time;
    u64 end_time;
    int exit_code;
} command_history_t;

int shell_init(void);
int shell_execute_command(const char *cmd_line);
int shell_parse_command(const char *cmd_line, char ***args, u32 *arg_count);
const char **shell_auto_complete(const char *prefix, u32 *count);
int shell_suggest_command(const char *typed, char ***suggestions, u32 *count);
int shell_create_alias(const char *alias_name, const char *command);
int shell_remove_alias(const char *alias_name);
int shell_list_aliases(alias_t **aliases, u32 *count);
command_history_t **shell_get_command_history(u64 limit, u32 *count);
int shell_clear_history(void);
int shell_execute_script(const char *script_path);
int shell_set_working_directory(const char *path);
const char *shell_get_working_directory(void);
int shell_enable_syntax_highlighting(void);

#endif
