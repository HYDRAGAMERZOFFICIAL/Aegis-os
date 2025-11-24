#ifndef AEGIS_SANDBOX_MANAGER_H
#define AEGIS_SANDBOX_MANAGER_H

#include <kernel/types.h>

typedef enum {
    PERM_NONE = 0,
    PERM_CAMERA = (1 << 0),
    PERM_MICROPHONE = (1 << 1),
    PERM_LOCATION = (1 << 2),
    PERM_CONTACTS = (1 << 3),
    PERM_CALENDAR = (1 << 4),
    PERM_PHOTOS = (1 << 5),
    PERM_NETWORK = (1 << 6),
    PERM_USB = (1 << 7),
    PERM_CLIPBOARD = (1 << 8)
} sandbox_permission_t;

typedef struct {
    u64 sandbox_id;
    u64 app_pid;
    const char *app_name;
    sandbox_permission_t permissions;
    u64 memory_limit;
    u64 cpu_limit;
    bool network_isolated;
    bool file_system_readonly;
    char **allowed_paths;
    u32 allowed_path_count;
} sandbox_t;

typedef struct {
    const char *key;
    const char *value;
} sandbox_policy_rule_t;

int sandbox_mgr_init(void);
sandbox_t *sandbox_mgr_create_sandbox(const char *app_name, u64 memory_limit, u64 cpu_limit);
int sandbox_mgr_destroy_sandbox(u64 sandbox_id);
int sandbox_mgr_grant_permission(u64 sandbox_id, sandbox_permission_t perm);
int sandbox_mgr_revoke_permission(u64 sandbox_id, sandbox_permission_t perm);
int sandbox_mgr_set_resource_limit(u64 sandbox_id, u64 memory_limit, u64 cpu_limit);
int sandbox_mgr_allow_path(u64 sandbox_id, const char *path, bool readonly);
int sandbox_mgr_deny_path(u64 sandbox_id, const char *path);
int sandbox_mgr_isolate_network(u64 sandbox_id);
int sandbox_mgr_monitor_sandbox(u64 sandbox_id);
sandbox_t **sandbox_mgr_get_active_sandboxes(u32 *count);
int sandbox_mgr_set_policy(const char *app_name, sandbox_policy_rule_t *rules, u32 rule_count);
int sandbox_mgr_apply_default_policy(u64 sandbox_id);

#endif
