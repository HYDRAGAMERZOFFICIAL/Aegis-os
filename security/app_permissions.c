#include <security/app_permissions.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_APPS 128
#define MAX_LOGS 128

static app_permission_profile_t g_profiles[MAX_APPS] = {0};
static u32 g_profile_count = 0;

static permission_access_log_t g_logs[MAX_LOGS] = {0};
static permission_access_log_t *g_log_ptrs[MAX_LOGS] = {0};
static u32 g_log_count = 0;

int appm_init(void) {
    g_profile_count = 0;
    g_log_count = 0;
    return 0;
}

int appm_register_app(u64 app_id, const char *app_name) {
    if (!app_name || g_profile_count >= MAX_APPS) return -1;
    for (u32 i = 0; i < g_profile_count; i++) {
        if (g_profiles[i].app_id == app_id) {
            g_profiles[i].app_name = app_name;
            return 0;
        }
    }
    app_permission_profile_t *p = &g_profiles[g_profile_count++];
    p->app_id = app_id;
    p->app_name = app_name;
    p->permissions = APP_PERM_NONE;
    p->requested_permissions = APP_PERM_NONE;
    p->user_prompt_pending = false;
    return 0;
}

int appm_request_permission(u64 app_id, app_permission_t perm) {
    for (u32 i = 0; i < g_profile_count; i++) {
        if (g_profiles[i].app_id == app_id) {
            g_profiles[i].requested_permissions |= perm;
            g_profiles[i].user_prompt_pending = true;
            return 0;
        }
    }
    return -1;
}

int appm_grant_permission(u64 app_id, app_permission_t perm) {
    for (u32 i = 0; i < g_profile_count; i++) {
        if (g_profiles[i].app_id == app_id) {
            g_profiles[i].permissions |= perm;
            g_profiles[i].requested_permissions &= ~perm;
            g_profiles[i].user_prompt_pending = false;

            if (g_log_count < MAX_LOGS) {
                permission_access_log_t *log = &g_logs[g_log_count];
                log->log_id = g_log_count + 1;
                log->app_id = app_id;
                log->permission = perm;
                log->timestamp = (u64)time(NULL);
                log->granted = true;
                g_log_ptrs[g_log_count] = log;
                g_log_count++;
            }
            return 0;
        }
    }
    return -1;
}

int appm_deny_permission(u64 app_id, app_permission_t perm) {
    for (u32 i = 0; i < g_profile_count; i++) {
        if (g_profiles[i].app_id == app_id) {
            g_profiles[i].permissions &= ~perm;
            g_profiles[i].requested_permissions &= ~perm;
            g_profiles[i].user_prompt_pending = false;

            if (g_log_count < MAX_LOGS) {
                permission_access_log_t *log = &g_logs[g_log_count];
                log->log_id = g_log_count + 1;
                log->app_id = app_id;
                log->permission = perm;
                log->timestamp = (u64)time(NULL);
                log->granted = false;
                g_log_ptrs[g_log_count] = log;
                g_log_count++;
            }
            return 0;
        }
    }
    return -1;
}

int appm_revoke_permission(u64 app_id, app_permission_t perm) {
    return appm_deny_permission(app_id, perm);
}

int appm_check_permission(u64 app_id, app_permission_t perm) {
    for (u32 i = 0; i < g_profile_count; i++) {
        if (g_profiles[i].app_id == app_id) {
            return (g_profiles[i].permissions & perm) == perm ? 0 : -1;
        }
    }
    return -1;
}

int appm_prompt_user(u64 app_id, app_permission_t perm) {
    return appm_request_permission(app_id, perm);
}

permission_state_t appm_get_permission_state(u64 app_id, app_permission_t perm) {
    for (u32 i = 0; i < g_profile_count; i++) {
        if (g_profiles[i].app_id == app_id) {
            if ((g_profiles[i].permissions & perm) == perm) return PERM_STATE_GRANTED;
            if ((g_profiles[i].requested_permissions & perm) == perm) return PERM_STATE_PENDING;
            return PERM_STATE_DENIED;
        }
    }
    return PERM_STATE_DENIED;
}

permission_access_log_t **appm_get_access_logs(u64 app_id, u64 start_time, u64 end_time, u32 *count) {
    if (!count) return NULL;
    (void)app_id;
    (void)start_time;
    (void)end_time;
    *count = g_log_count;
    return g_log_ptrs;
}

int appm_set_default_policy(u64 app_id, app_permission_t default_perms) {
    for (u32 i = 0; i < g_profile_count; i++) {
        if (g_profiles[i].app_id == app_id) {
            g_profiles[i].permissions = default_perms;
            return 0;
        }
    }
    return -1;
}

int appm_deny_all_permissions(u64 app_id) {
    return appm_set_default_policy(app_id, APP_PERM_NONE);
}

app_permission_profile_t *appm_get_app_profile(u64 app_id) {
    for (u32 i = 0; i < g_profile_count; i++) {
        if (g_profiles[i].app_id == app_id) {
            return &g_profiles[i];
        }
    }
    return NULL;
}

int appm_shutdown(void) { return 0; }
