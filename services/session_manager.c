#include <string.h>

int session_init(void) { return 0; }
int session_shutdown(void) { return 0; }
int session_create(const char* username) { return (username) ? 0 : -1; }
int session_destroy(int session_id) { return 0; }
int session_login(const char* username, const char* password) { return (username && password) ? 0 : -1; }
int session_logout(int session_id) { return 0; }
int session_lock(int session_id) { return 0; }
int session_unlock(int session_id, const char* password) { return (password) ? 0 : -1; }
int session_get_current_user(void) { return 0; }
int session_switch_user(int session_id) { return 0; }
