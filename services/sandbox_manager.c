#include <string.h>

int sandbox_init(void) { return 0; }
int sandbox_shutdown(void) { return 0; }
int sandbox_create(const char* name, int level) { return (name) ? 0 : -1; }
int sandbox_destroy(int sandbox_id) { return 0; }
int sandbox_launch_app(int sandbox_id, const char* app_path) { return (app_path) ? 0 : -1; }
int sandbox_set_resource_limits(int sandbox_id, void* limits) { return (limits) ? 0 : -1; }
int sandbox_enable_network(int sandbox_id) { return 0; }
int sandbox_disable_network(int sandbox_id) { return 0; }
int sandbox_allow_file_access(int sandbox_id, const char* path) { return (path) ? 0 : -1; }
int sandbox_deny_file_access(int sandbox_id, const char* path) { return (path) ? 0 : -1; }
int sandbox_get_status(int sandbox_id) { return 0; }
