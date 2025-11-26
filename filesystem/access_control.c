#include <filesystem/access_control.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    acl_entry_t *acl_entries;
    u32 acl_count;
    role_t *roles;
    u32 role_count;
    file_permission_t default_permissions;
    u32 max_acls;
    u32 max_roles;
} acl_state_t;

static acl_state_t acl_state = {0};

int acl_init(void)
{
    acl_state.max_acls = 8192;
    acl_state.max_roles = 512;
    acl_state.acl_count = 0;
    acl_state.role_count = 0;
    acl_state.default_permissions = PERM_READ | PERM_WRITE;

    acl_state.acl_entries = (acl_entry_t *)calloc(acl_state.max_acls, sizeof(acl_entry_t));
    if (!acl_state.acl_entries) return -1;

    acl_state.roles = (role_t *)calloc(acl_state.max_roles, sizeof(role_t));
    if (!acl_state.roles) return -1;

    return 0;
}

int acl_set_permissions(const char *path, u64 principal_id, principal_type_t type, file_permission_t perms)
{
    if (!path) return -1;

    if (acl_state.acl_count >= acl_state.max_acls) return -1;

    acl_entry_t *entry = &acl_state.acl_entries[acl_state.acl_count];
    entry->acl_id = acl_state.acl_count;
    entry->file_path = path;
    entry->principal_id = principal_id;
    entry->principal_type = type;
    entry->permissions = perms;
    entry->inherited = 0;
    entry->expires_at = 0;

    acl_state.acl_count++;

    return 0;
}

int acl_grant_permission(const char *path, u64 principal_id, file_permission_t perm)
{
    if (!path) return -1;

    for (u32 i = 0; i < acl_state.acl_count; i++) {
        if (strcmp(acl_state.acl_entries[i].file_path, path) == 0 &&
            acl_state.acl_entries[i].principal_id == principal_id) {
            acl_state.acl_entries[i].permissions |= perm;
            return 0;
        }
    }

    return acl_set_permissions(path, principal_id, PRINCIPAL_TYPE_USER, perm);
}

int acl_revoke_permission(const char *path, u64 principal_id, file_permission_t perm)
{
    if (!path) return -1;

    for (u32 i = 0; i < acl_state.acl_count; i++) {
        if (strcmp(acl_state.acl_entries[i].file_path, path) == 0 &&
            acl_state.acl_entries[i].principal_id == principal_id) {
            acl_state.acl_entries[i].permissions &= ~perm;
            return 0;
        }
    }

    return -1;
}

int acl_check_permission(const char *path, u64 principal_id, file_permission_t perm)
{
    if (!path) return 0;

    for (u32 i = 0; i < acl_state.acl_count; i++) {
        if (strcmp(acl_state.acl_entries[i].file_path, path) == 0 &&
            acl_state.acl_entries[i].principal_id == principal_id) {
            return (acl_state.acl_entries[i].permissions & perm) != 0 ? 1 : 0;
        }
    }

    return 0;
}

int acl_create_role(const char *role_name, file_permission_t perms)
{
    if (!role_name) return -1;

    if (acl_state.role_count >= acl_state.max_roles) return -1;

    role_t *role = &acl_state.roles[acl_state.role_count];
    role->role_id = acl_state.role_count;
    role->role_name = role_name;
    role->permissions = perms;
    role->member_ids = (u32 *)calloc(256, sizeof(u32));
    if (!role->member_ids) return -1;
    role->member_count = 0;

    acl_state.role_count++;

    return 0;
}

int acl_add_role_member(u64 role_id, u64 principal_id)
{
    if (role_id >= acl_state.role_count) return -1;

    role_t *role = &acl_state.roles[role_id];
    if (role->member_count >= 256) return -1;

    role->member_ids[role->member_count++] = principal_id;

    return 0;
}

int acl_remove_role_member(u64 role_id, u64 principal_id)
{
    if (role_id >= acl_state.role_count) return -1;

    role_t *role = &acl_state.roles[role_id];

    for (u32 i = 0; i < role->member_count; i++) {
        if (role->member_ids[i] == principal_id) {
            role->member_ids[i] = role->member_ids[role->member_count - 1];
            role->member_count--;
            return 0;
        }
    }

    return -1;
}

acl_entry_t **acl_get_entries(const char *path, u32 *count)
{
    if (!path || !count) return NULL;

    acl_entry_t **result = (acl_entry_t **)calloc(acl_state.acl_count, sizeof(acl_entry_t *));
    if (!result) return NULL;

    u32 result_count = 0;
    for (u32 i = 0; i < acl_state.acl_count; i++) {
        if (strcmp(acl_state.acl_entries[i].file_path, path) == 0) {
            result[result_count++] = &acl_state.acl_entries[i];
        }
    }

    *count = result_count;
    return result;
}

int acl_inherit_permissions(const char *parent_path, const char *child_path)
{
    if (!parent_path || !child_path) return -1;

    for (u32 i = 0; i < acl_state.acl_count; i++) {
        if (strcmp(acl_state.acl_entries[i].file_path, parent_path) == 0) {
            acl_entry_t *child_entry = &acl_state.acl_entries[acl_state.acl_count];
            if (acl_state.acl_count >= acl_state.max_acls) return -1;

            memcpy(child_entry, &acl_state.acl_entries[i], sizeof(acl_entry_t));
            child_entry->file_path = child_path;
            child_entry->inherited = 1;
            acl_state.acl_count++;
        }
    }

    return 0;
}

int acl_set_default_permissions(file_permission_t default_perms)
{
    acl_state.default_permissions = default_perms;
    return 0;
}

int acl_audit_access(const char *path, u64 principal_id)
{
    if (!path) return -1;
    return 0;
}

int acl_enable_mandatory_access_control(void)
{
    return 0;
}
