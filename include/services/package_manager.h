#ifndef AEGIS_PACKAGE_MANAGER_H
#define AEGIS_PACKAGE_MANAGER_H

#include <kernel/types.h>

typedef enum {
    PKG_STATE_AVAILABLE,
    PKG_STATE_INSTALLED,
    PKG_STATE_UPDATING,
    PKG_STATE_BROKEN,
    PKG_STATE_REMOVED
} package_state_t;

typedef struct {
    u64 package_id;
    const char *name;
    const char *version;
    package_state_t state;
    u64 size;
    char **dependencies;
    u32 dependency_count;
    bool containerized;
    u8 *manifest_hash;
} package_t;

typedef struct {
    const char *name;
    const char *version;
    u64 download_size;
    u8 signature[256];
    bool available;
} package_info_t;

int pkgmgr_init(void);
package_info_t **pkgmgr_search_packages(const char *query, u32 *count);
int pkgmgr_install_package(const char *name, const char *version);
int pkgmgr_uninstall_package(u64 package_id);
int pkgmgr_update_package(u64 package_id);
int pkgmgr_verify_package(u64 package_id);
int pkgmgr_scan_code(u64 package_id);
package_t **pkgmgr_get_installed_packages(u32 *count);
int pkgmgr_resolve_dependencies(const char *name, char ***deps, u32 *dep_count);
int pkgmgr_enable_auto_update(u64 package_id);
package_state_t pkgmgr_get_package_state(u64 package_id);
int pkgmgr_containerize_app(u64 package_id);
int pkgmgr_get_update_count(u32 *count);

#endif
