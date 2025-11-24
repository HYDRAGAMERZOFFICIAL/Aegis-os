#include <string.h>

int service_manager_init(void) { return 0; }
int service_manager_shutdown(void) { return 0; }
int service_register(const char* name, void* handler) { return (name && handler) ? 0 : -1; }
int service_unregister(const char* name) { return name ? 0 : -1; }
int service_start(const char* name) { return name ? 0 : -1; }
int service_stop(const char* name) { return name ? 0 : -1; }
int service_restart(const char* name) { return name ? 0 : -1; }
int service_get_status(const char* name) { return name ? 0 : -1; }
int service_enable(const char* name) { return name ? 0 : -1; }
int service_disable(const char* name) { return name ? 0 : -1; }
int service_list_all(void) { return 0; }
