#ifndef AEGIS_ACCESS_CONTROL_H
#define AEGIS_ACCESS_CONTROL_H

#include <kernel/types.h>

typedef enum {
    PERM_READ = (1 << 0),
    PERM_WRITE = (1 << 1),
    PERM_EXECUTE = (1 << 2),
    PERM_DELETE = (1 << 3),
    PERM_ADMIN = (1 << 4)
} file_permission_t;

typedef enum {
    PRINCIPAL_TYPE_USER,
    PRINCIPAL_TYPE_GROUP,
    PRINCIPAL_TYPE_ROLE
} principal_type_t;

typedef struct {
    u64 acl_id;
    const char *file_path;
    u64 principal_id;
    principal_type_t principal_type;
    file_permission_t permissions;
    bool inherited;
    u64 expires_at;
} acl_entry_t;

typedef struct {
    u64 role_id;
    const char *role_name;
    file_permission_t permissions;
    u32 *member_ids;
    u32 member_count;
} role_t;

int acl_init(void);
int acl_set_permissions(const char *path, u64 principal_id, principal_type_t type, file_permission_t perms);
int acl_grant_permission(const char *path, u64 principal_id, file_permission_t perm);
int acl_revoke_permission(const char *path, u64 principal_id, file_permission_t perm);
int acl_check_permission(const char *path, u64 principal_id, file_permission_t perm);
int acl_create_role(const char *role_name, file_permission_t perms);
int acl_add_role_member(u64 role_id, u64 principal_id);
int acl_remove_role_member(u64 role_id, u64 principal_id);
acl_entry_t **acl_get_entries(const char *path, u32 *count);
int acl_inherit_permissions(const char *parent_path, const char *child_path);
int acl_set_default_permissions(file_permission_t default_perms);
int acl_audit_access(const char *path, u64 principal_id);
int acl_enable_mandatory_access_control(void);

#endif
