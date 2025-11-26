#include <userland/aegis_shell.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    alias_t *aliases;
    u32 alias_count;
    command_history_t *history;
    u32 history_count;
    char *working_directory;
    u32 max_aliases;
    u32 max_history;
    u32 syntax_highlighting_enabled;
} shell_state_t;

static shell_state_t shell_state = {0};

int shell_init(void)
{
    shell_state.max_aliases = 256;
    shell_state.max_history = 4096;
    shell_state.alias_count = 0;
    shell_state.history_count = 0;
    shell_state.syntax_highlighting_enabled = 0;

    shell_state.aliases = (alias_t *)calloc(shell_state.max_aliases, sizeof(alias_t));
    if (!shell_state.aliases) return -1;

    shell_state.history = (command_history_t *)calloc(shell_state.max_history, sizeof(command_history_t));
    if (!shell_state.history) return -1;

    shell_state.working_directory = (char *)calloc(256, sizeof(char));
    if (!shell_state.working_directory) return -1;

    strcpy(shell_state.working_directory, "/home");

    return 0;
}

int shell_execute_command(const char *cmd_line)
{
    if (!cmd_line) return -1;

    if (shell_state.history_count >= shell_state.max_history) return -1;

    command_history_t *cmd = &shell_state.history[shell_state.history_count];
    cmd->command_line = (char *)cmd_line;
    cmd->start_time = 0;
    cmd->end_time = 0;
    cmd->exit_code = 0;

    shell_state.history_count++;

    return 0;
}

int shell_parse_command(const char *cmd_line, char ***args, u32 *arg_count)
{
    if (!cmd_line || !args || !arg_count) return -1;

    char **arg_list = (char **)calloc(64, sizeof(char *));
    if (!arg_list) return -1;

    u32 count = 0;
    const char *current = cmd_line;

    while (*current && count < 64) {
        if (*current == ' ') {
            current++;
            continue;
        }

        const char *start = current;
        while (*current && *current != ' ') {
            current++;
        }

        u64 len = current - start;
        arg_list[count] = (char *)calloc(len + 1, sizeof(char));
        if (!arg_list[count]) return -1;

        strncpy(arg_list[count], start, len);
        arg_list[count][len] = '\0';

        count++;
    }

    *args = arg_list;
    *arg_count = count;

    return 0;
}

const char **shell_auto_complete(const char *prefix, u32 *count)
{
    if (!prefix || !count) return NULL;

    const char **completions = (const char **)calloc(32, sizeof(const char *));
    if (!completions) return NULL;

    *count = 0;
    return completions;
}

int shell_suggest_command(const char *typed, char ***suggestions, u32 *count)
{
    if (!typed || !suggestions || !count) return -1;

    char **sugg_list = (char **)calloc(32, sizeof(char *));
    if (!sugg_list) return -1;

    *suggestions = sugg_list;
    *count = 0;

    return 0;
}

int shell_create_alias(const char *alias_name, const char *command)
{
    if (!alias_name || !command) return -1;

    if (shell_state.alias_count >= shell_state.max_aliases) return -1;

    alias_t *alias = &shell_state.aliases[shell_state.alias_count];
    alias->name = alias_name;
    alias->expansion = command;

    shell_state.alias_count++;

    return 0;
}

int shell_remove_alias(const char *alias_name)
{
    if (!alias_name) return -1;

    for (u32 i = 0; i < shell_state.alias_count; i++) {
        if (strcmp(shell_state.aliases[i].name, alias_name) == 0) {
            shell_state.aliases[i] = shell_state.aliases[shell_state.alias_count - 1];
            shell_state.alias_count--;
            return 0;
        }
    }

    return -1;
}

int shell_list_aliases(alias_t **aliases, u32 *count)
{
    if (!aliases || !count) return -1;

    *aliases = shell_state.aliases;
    *count = shell_state.alias_count;

    return 0;
}

command_history_t **shell_get_command_history(u64 limit, u32 *count)
{
    if (!count) return NULL;

    command_history_t **result = (command_history_t **)calloc(shell_state.history_count, sizeof(command_history_t *));
    if (!result) return NULL;

    u32 result_count = 0;
    u32 start = shell_state.history_count > (u32)limit ? shell_state.history_count - (u32)limit : 0;

    for (u32 i = start; i < shell_state.history_count; i++) {
        result[result_count++] = &shell_state.history[i];
    }

    *count = result_count;
    return result;
}

int shell_clear_history(void)
{
    shell_state.history_count = 0;
    return 0;
}

int shell_execute_script(const char *script_path)
{
    if (!script_path) return -1;

    return 0;
}

int shell_set_working_directory(const char *path)
{
    if (!path) return -1;

    if (strlen(path) >= 256) return -1;

    strcpy(shell_state.working_directory, path);

    return 0;
}

const char *shell_get_working_directory(void)
{
    return shell_state.working_directory;
}

int shell_enable_syntax_highlighting(void)
{
    shell_state.syntax_highlighting_enabled = 1;
    return 0;
}
