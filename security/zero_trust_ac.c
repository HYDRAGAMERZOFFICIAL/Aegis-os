#include <security/zero_trust_ac.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_RULES 256
#define MAX_LOGS 128

static access_rule_t g_rules[MAX_RULES] = {0};
static access_rule_t *g_rule_ptrs[MAX_RULES] = {0};
static u32 g_rule_count = 0;
static rule_action_t g_default_action = RULE_ACTION_DENY;
static u32 g_denials_count = 0;

int ztac_init(void) {
    g_rule_count = 0;
    g_default_action = RULE_ACTION_DENY;
    g_denials_count = 0;

    // Default system admin rule
    ztac_create_rule(1, RESOURCE_TYPE_FILE, "/system", ACTION_READ, RULE_ACTION_ALLOW);
    ztac_create_rule(1, RESOURCE_TYPE_PROCESS, "*", ACTION_ADMIN, RULE_ACTION_ALLOW);
    return 0;
}

int ztac_create_rule(u64 principal_id, resource_type_t res_type, const char *resource_path, action_type_t action, rule_action_t rule_action) {
    if (g_rule_count >= MAX_RULES) return -1;
    access_rule_t *r = &g_rules[g_rule_count];
    r->rule_id = g_rule_count + 1;
    r->principal_id = principal_id;
    r->resource_type = res_type;
    r->resource_path = resource_path;
    r->action = action;
    r->rule_action = rule_action;
    r->expires_at = 0;
    r->requires_mfa = false;

    g_rule_ptrs[g_rule_count] = r;
    g_rule_count++;
    return (int)r->rule_id;
}

int ztac_delete_rule(u64 rule_id) {
    for (u32 i = 0; i < g_rule_count; i++) {
        if (g_rules[i].rule_id == rule_id) {
            for (u32 j = i; j < g_rule_count - 1; j++) {
                g_rules[j] = g_rules[j + 1];
                g_rule_ptrs[j] = &g_rules[j];
            }
            g_rule_count--;
            return 0;
        }
    }
    return -1;
}

int ztac_check_access(u64 principal_id, resource_type_t res_type, const char *resource, action_type_t action) {
    for (u32 i = 0; i < g_rule_count; i++) {
        if (g_rules[i].principal_id == principal_id &&
            g_rules[i].resource_type == res_type &&
            g_rules[i].action == action) {
            if (g_rules[i].resource_path == NULL || strcmp(g_rules[i].resource_path, "*") == 0 ||
                (resource && strcmp(g_rules[i].resource_path, resource) == 0)) {
                return (g_rules[i].rule_action == RULE_ACTION_ALLOW) ? 0 : -1;
            }
        }
    }
    if (g_default_action == RULE_ACTION_DENY) {
        g_denials_count++;
        return -1;
    }
    return 0;
}

int ztac_challenge_access(u64 principal_id, resource_type_t res_type, const char *resource) {
    (void)principal_id;
    (void)res_type;
    (void)resource;
    return 0;
}

int ztac_grant_temporary_access(u64 principal_id, const char *resource, u64 duration_seconds) {
    int id = ztac_create_rule(principal_id, RESOURCE_TYPE_FILE, resource, ACTION_READ, RULE_ACTION_ALLOW);
    if (id > 0 && (u32)id <= g_rule_count) {
        g_rules[id - 1].expires_at = (u64)time(NULL) + duration_seconds;
    }
    return 0;
}

int ztac_revoke_access(u64 principal_id, const char *resource) {
    for (u32 i = 0; i < g_rule_count; i++) {
        if (g_rules[i].principal_id == principal_id &&
            g_rules[i].resource_path && strcmp(g_rules[i].resource_path, resource) == 0) {
            return ztac_delete_rule(g_rules[i].rule_id);
        }
    }
    return -1;
}

access_rule_t **ztac_get_rules_for_principal(u64 principal_id, u32 *count) {
    if (!count) return NULL;
    static access_rule_t *matched[MAX_RULES];
    u32 m_count = 0;
    for (u32 i = 0; i < g_rule_count; i++) {
        if (g_rules[i].principal_id == principal_id) {
            matched[m_count++] = &g_rules[i];
        }
    }
    *count = m_count;
    return matched;
}

int ztac_enable_mfa_requirement(u64 rule_id) {
    for (u32 i = 0; i < g_rule_count; i++) {
        if (g_rules[i].rule_id == rule_id) {
            g_rules[i].requires_mfa = true;
            return 0;
        }
    }
    return -1;
}

int ztac_log_access_attempt(u64 principal_id, const char *resource, bool granted) {
    (void)principal_id;
    (void)resource;
    if (!granted) g_denials_count++;
    return 0;
}

int ztac_audit_all_denials(void) {
    return (int)g_denials_count;
}

int ztac_set_default_policy(rule_action_t default_action) {
    g_default_action = default_action;
    return 0;
}

/* Compatibility functions */
int zt_init(void) { return ztac_init(); }
int zt_shutdown(void) { return 0; }
int zt_register_principal(const char* name, uint32_t type) { (void)name; (void)type; return 0; }
int zt_unregister_principal(uint32_t principal_id) { (void)principal_id; return 0; }
int zt_create_policy(const char* name, uint32_t effect) { (void)name; (void)effect; return 0; }
int zt_delete_policy(uint32_t policy_id) { (void)policy_id; return 0; }
int zt_add_statement_to_policy(uint32_t policy_id, uint32_t principal_id, uint32_t action) { (void)policy_id; (void)principal_id; (void)action; return 0; }
int zt_verify_access(uint32_t principal_id, uint32_t action, uint32_t resource_id) { (void)principal_id; (void)action; (void)resource_id; return 1; }
int zt_audit_access(uint32_t principal_id, uint32_t action, uint32_t allowed) { (void)principal_id; (void)action; (void)allowed; return 0; }
int zt_enable_continuous_verification(void) { return 0; }
int zt_disable_continuous_verification(void) { return 0; }
int zt_context_aware_check(uint32_t principal_id, uint32_t action, void* context) { (void)principal_id; (void)action; (void)context; return 1; }
int zt_require_mfa(uint32_t principal_id) { (void)principal_id; return 0; }
int zt_verify_mfa(uint32_t principal_id, const char* token) { (void)principal_id; (void)token; return 0; }
