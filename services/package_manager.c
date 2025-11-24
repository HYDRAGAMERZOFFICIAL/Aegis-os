#include <string.h>

int pkgmgr_init(void) { return 0; }
int pkgmgr_shutdown(void) { return 0; }
int pkgmgr_install(const char* package_name) { return (package_name) ? 0 : -1; }
int pkgmgr_uninstall(const char* package_name) { return (package_name) ? 0 : -1; }
int pkgmgr_update(const char* package_name) { return (package_name) ? 0 : -1; }
int pkgmgr_search(const char* query) { return (query) ? 0 : -1; }
int pkgmgr_list_installed(void) { return 0; }
int pkgmgr_get_updates(void) { return 0; }
int pkgmgr_update_all(void) { return 0; }
int pkgmgr_add_repository(const char* repo_url) { return (repo_url) ? 0 : -1; }
int pkgmgr_remove_repository(const char* repo_url) { return (repo_url) ? 0 : -1; }
