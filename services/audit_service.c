#include <string.h>

int audit_init(void) { return 0; }
int audit_shutdown(void) { return 0; }
int audit_log_event(const char* event_type, const char* details) { return (event_type) ? 0 : -1; }
int audit_log_syscall(int syscall_num, int result) { return 0; }
int audit_log_file_access(const char* path, int flags) { return (path) ? 0 : -1; }
int audit_log_network_access(const char* host, int port) { return (host) ? 0 : -1; }
int audit_log_permission_change(const char* resource, const char* permission) { return (resource) ? 0 : -1; }
int audit_get_events(void* buffer, int max_count) { return (buffer) ? 0 : -1; }
int audit_clear_events(void) { return 0; }
int audit_enable_logging(void) { return 0; }
int audit_disable_logging(void) { return 0; }
