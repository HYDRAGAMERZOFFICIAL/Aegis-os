#include <string.h>

int update_init(void) { return 0; }
int update_shutdown(void) { return 0; }
int update_check_available(void) { return 0; }
int update_download(const char* version) { return (version) ? 0 : -1; }
int update_install(const char* version) { return (version) ? 0 : -1; }
int update_install_later(void) { return 0; }
int update_get_current_version(void) { return 0; }
int update_rollback(void) { return 0; }
int update_auto_update_enable(void) { return 0; }
int update_auto_update_disable(void) { return 0; }
int update_schedule_update(int hour, int minute) { return 0; }
