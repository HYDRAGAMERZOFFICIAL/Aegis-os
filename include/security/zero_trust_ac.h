#ifndef AEGIS_ZERO_TRUST_AC_H
#define AEGIS_ZERO_TRUST_AC_H

#include <kernel/types.h>

typedef enum {
    RESOURCE_TYPE_FILE,
    RESOURCE_TYPE_DIRECTORY,
    RESOURCE_TYPE_DEVICE,
    RESOURCE_TYPE_NETWORK,
    RESOURCE_TYPE_PROCESS,
    RESOURCE_TYPE_IPC,
    RESOURCE_TYPE_API
} resource_type_t;

typedef enum {
    ACTION_READ,
    ACTION_WRITE,
    ACTION_EXECUTE,
    ACTION_DELETE,
    ACTION_MODIFY,
    ACTION_ADMIN
} action_type_t;

typedef enum {
    RULE_ACTION_ALLOW,
    RULE_ACTION_DENY,
    RULE_ACTION_CHALLENGE,
    RULE_ACTION_LOG
} rule_action_t;

typedef struct {
    u64 rule_id;
    u64 principal_id;
    resource_type_t resource_type;
    const char *resource_path;
    action_type_t action;
    rule_action_t rule_action;
    u64 expires_at;
    bool requires_mfa;
} access_rule_t;

int ztac_init(void);
int ztac_create_rule(u64 principal_id, resource_type_t res_type, const char *resource_path, action_type_t action, rule_action_t rule_action);
int ztac_delete_rule(u64 rule_id);
int ztac_check_access(u64 principal_id, resource_type_t res_type, const char *resource, action_type_t action);
int ztac_challenge_access(u64 principal_id, resource_type_t res_type, const char *resource);
int ztac_grant_temporary_access(u64 principal_id, const char *resource, u64 duration_seconds);
int ztac_revoke_access(u64 principal_id, const char *resource);
access_rule_t **ztac_get_rules_for_principal(u64 principal_id, u32 *count);
int ztac_enable_mfa_requirement(u64 rule_id);
int ztac_log_access_attempt(u64 principal_id, const char *resource, bool granted);
int ztac_audit_all_denials(void);
int ztac_set_default_policy(rule_action_t default_action);

#endif
