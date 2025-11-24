#ifndef AEGIS_APP_PERMISSIONS_H
#define AEGIS_APP_PERMISSIONS_H

#include <kernel/types.h>

typedef enum {
    APP_PERM_NONE = 0,
    APP_PERM_CAMERA = (1 << 0),
    APP_PERM_MICROPHONE = (1 << 1),
    APP_PERM_LOCATION = (1 << 2),
    APP_PERM_CONTACTS = (1 << 3),
    APP_PERM_CALENDAR = (1 << 4),
    APP_PERM_PHOTOS = (1 << 5),
    APP_PERM_STORAGE = (1 << 6),
    APP_PERM_CLIPBOARD = (1 << 7),
    APP_PERM_BLUETOOTH = (1 << 8),
    APP_PERM_NFC = (1 << 9),
    APP_PERM_USB = (1 << 10),
    APP_PERM_PRINTER = (1 << 11),
    APP_PERM_NETWORK = (1 << 12),
    APP_PERM_SENSOR = (1 << 13)
} app_permission_t;

typedef enum {
    PERM_STATE_GRANTED,
    PERM_STATE_DENIED,
    PERM_STATE_PENDING,
    PERM_STATE_REVOKED
} permission_state_t;

typedef struct {
    u64 app_id;
    const char *app_name;
    app_permission_t permissions;
    app_permission_t requested_permissions;
    bool user_prompt_pending;
} app_permission_profile_t;

typedef struct {
    u64 log_id;
    u64 app_id;
    app_permission_t permission;
    u64 timestamp;
    bool granted;
} permission_access_log_t;

int appm_init(void);
int appm_register_app(u64 app_id, const char *app_name);
int appm_request_permission(u64 app_id, app_permission_t perm);
int appm_grant_permission(u64 app_id, app_permission_t perm);
int appm_deny_permission(u64 app_id, app_permission_t perm);
int appm_revoke_permission(u64 app_id, app_permission_t perm);
int appm_check_permission(u64 app_id, app_permission_t perm);
int appm_prompt_user(u64 app_id, app_permission_t perm);
permission_state_t appm_get_permission_state(u64 app_id, app_permission_t perm);
permission_access_log_t **appm_get_access_logs(u64 app_id, u64 start_time, u64 end_time, u32 *count);
int appm_set_default_policy(u64 app_id, app_permission_t default_perms);
int appm_deny_all_permissions(u64 app_id);
app_permission_profile_t *appm_get_app_profile(u64 app_id);

#endif
