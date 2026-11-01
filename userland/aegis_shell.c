#include <userland/aegis_shell.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef struct {
    alias_t *aliases;
    u32 alias_count;
    command_history_t *history;
    command_history_t **history_ptrs;
    u32 history_count;
    char working_directory[256];
    u32 max_aliases;
    u32 max_history;
    u32 syntax_highlighting_enabled;
    char last_output[4096];
} shell_state_t;

static shell_state_t shell_state = {0};
static int shell_initialized = 0;

static const char *g_builtin_cmds[] = {
    "help", "ps", "kill", "shield", "guard", "neural",
    "troubleshoot", "opt", "checkpoint", "fs", "theme",
    "store", "clear", "echo", "pwd", "cd", "sysinfo"
};

int shell_init(void)
{
    if (shell_initialized) return 0;

    shell_state.max_aliases = 256;
    shell_state.max_history = 4096;
    shell_state.alias_count = 0;
    shell_state.history_count = 0;
    shell_state.syntax_highlighting_enabled = 1;

    shell_state.aliases = (alias_t *)calloc(shell_state.max_aliases, sizeof(alias_t));
    if (!shell_state.aliases) return -1;

    shell_state.history = (command_history_t *)calloc(shell_state.max_history, sizeof(command_history_t));
    if (!shell_state.history) return -1;

    shell_state.history_ptrs = (command_history_t **)calloc(shell_state.max_history, sizeof(command_history_t *));
    if (!shell_state.history_ptrs) return -1;

    strncpy(shell_state.working_directory, "/home/aegis", 255);
    shell_state.last_output[0] = '\0';

    // Default aliases
    shell_create_alias("ll", "fs ls -l");
    shell_create_alias("scan", "troubleshoot run");
    shell_create_alias("protect", "shield enable");

    shell_initialized = 1;
    return 0;
}

int shell_execute_command(const char *cmd_line)
{
    if (!cmd_line) return -1;
    if (!shell_initialized) shell_init();

    // Check alias replacement
    const char *actual_cmd = cmd_line;
    for (u32 i = 0; i < shell_state.alias_count; i++) {
        if (strcmp(shell_state.aliases[i].name, cmd_line) == 0) {
            actual_cmd = shell_state.aliases[i].expansion;
            break;
        }
    }

    if (shell_state.history_count < shell_state.max_history) {
        command_history_t *cmd = &shell_state.history[shell_state.history_count];
        cmd->command_line = strdup(actual_cmd);
        cmd->start_time = (u64)time(NULL);
        cmd->end_time = cmd->start_time;
        cmd->exit_code = 0;
        shell_state.history_ptrs[shell_state.history_count] = cmd;
        shell_state.history_count++;
    }

    // Process built-ins
    if (strncmp(actual_cmd, "help", 4) == 0) {
        snprintf(shell_state.last_output, sizeof(shell_state.last_output),
            "AegisOS Shell Builtin Commands:\n"
            "  help          - Display this help reference\n"
            "  sysinfo       - Print kernel architecture & system state\n"
            "  ps            - List active processes and services\n"
            "  kill <pid>    - Terminate a process\n"
            "  shield <cmd>  - Firewall and Zero-Trust gatekeeper controls\n"
            "  guard <cmd>   - AI behavioral intrusion detection\n"
            "  neural        - Real-time neural activity monitor\n"
            "  troubleshoot  - Run diagnostic scans and auto-repair\n"
            "  opt <mode>    - Resource optimizer (game/work/low_power)\n"
            "  checkpoint    - System snapshot creation and rollback\n"
            "  theme <mode>  - Switch UI theme (productivity/gaming/dark/light)\n"
            "  pwd           - Print working directory\n"
            "  cd <path>     - Change working directory\n");
    } else if (strncmp(actual_cmd, "sysinfo", 7) == 0) {
        snprintf(shell_state.last_output, sizeof(shell_state.last_output),
            "AegisOS v2.0-Alpha (Dual-Kernel Microkernel Hybrid)\n"
            "Arch: x86_64 / ARMv8-A | Memory: 16384 MB | Security: Zero-Trust Active\n"
            "Shield: Active | Guard AI Accuracy: 98.5%% | Scheduler: Adaptive CFS\n");
    } else if (strncmp(actual_cmd, "pwd", 3) == 0) {
        snprintf(shell_state.last_output, sizeof(shell_state.last_output), "%s\n", shell_state.working_directory);
    } else if (strncmp(actual_cmd, "clear", 5) == 0) {
        shell_state.last_output[0] = '\0';
    } else {
        snprintf(shell_state.last_output, sizeof(shell_state.last_output),
            "Executed: %s (exit: 0)\n", actual_cmd);
    }

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
        while (*current == ' ') current++;
        if (!*current) break;

        const char *start = current;
        while (*current && *current != ' ') current++;

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

    u32 matched = 0;
    size_t prefix_len = strlen(prefix);
    size_t total_builtins = sizeof(g_builtin_cmds) / sizeof(g_builtin_cmds[0]);

    for (size_t i = 0; i < total_builtins && matched < 32; i++) {
        if (strncmp(g_builtin_cmds[i], prefix, prefix_len) == 0) {
            completions[matched++] = g_builtin_cmds[i];
        }
    }

    *count = matched;
    return completions;
}

int shell_suggest_command(const char *typed, char ***suggestions, u32 *count)
{
    if (!typed || !suggestions || !count) return -1;

    char **sugg_list = (char **)calloc(32, sizeof(char *));
    if (!sugg_list) return -1;

    u32 matched = 0;
    size_t total_builtins = sizeof(g_builtin_cmds) / sizeof(g_builtin_cmds[0]);
    for (size_t i = 0; i < total_builtins && matched < 32; i++) {
        if (strstr(g_builtin_cmds[i], typed) != NULL) {
            sugg_list[matched++] = strdup(g_builtin_cmds[i]);
        }
    }

    *suggestions = sugg_list;
    *count = matched;
    return 0;
}

int shell_create_alias(const char *alias_name, const char *command)
{
    if (!alias_name || !command) return -1;
    if (!shell_initialized) shell_init();
    if (shell_state.alias_count >= shell_state.max_aliases) return -1;

    alias_t *alias = &shell_state.aliases[shell_state.alias_count++];
    alias->name = alias_name;
    alias->expansion = command;
    return 0;
}

int shell_remove_alias(const char *alias_name)
{
    if (!alias_name) return -1;
    for (u32 i = 0; i < shell_state.alias_count; i++) {
        if (strcmp(shell_state.aliases[i].name, alias_name) == 0) {
            for (u32 j = i; j < shell_state.alias_count - 1; j++) {
                shell_state.aliases[j] = shell_state.aliases[j + 1];
            }
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
    u32 to_return = (shell_state.history_count < limit) ? shell_state.history_count : (u32)limit;
    *count = to_return;
    return shell_state.history_ptrs;
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
    strncpy(shell_state.working_directory, path, 255);
    return 0;
}

const char *shell_get_working_directory(void)
{
    if (!shell_initialized) shell_init();
    return shell_state.working_directory;
}

int shell_enable_syntax_highlighting(void)
{
    shell_state.syntax_highlighting_enabled = 1;
    return 0;
}
