#include <string.h>

int logging_init(void) { return 0; }
int logging_shutdown(void) { return 0; }
int logging_write(const char* message) { return (message) ? 0 : -1; }
int logging_write_debug(const char* message) { return (message) ? 0 : -1; }
int logging_write_error(const char* message) { return (message) ? 0 : -1; }
int logging_write_warning(const char* message) { return (message) ? 0 : -1; }
int logging_write_info(const char* message) { return (message) ? 0 : -1; }
int logging_flush(void) { return 0; }
int logging_set_level(int level) { return 0; }
int logging_rotate_log(void) { return 0; }
int logging_clear_log(void) { return 0; }
