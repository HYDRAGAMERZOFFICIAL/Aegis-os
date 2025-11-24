#include <string.h>

int netmgr_init(void) { return 0; }
int netmgr_shutdown(void) { return 0; }
int netmgr_connect_wifi(const char* ssid, const char* password) { return (ssid) ? 0 : -1; }
int netmgr_disconnect_wifi(void) { return 0; }
int netmgr_scan_networks(void) { return 0; }
int netmgr_list_connected(void) { return 0; }
int netmgr_set_static_ip(const char* ip, const char* gateway) { return (ip) ? 0 : -1; }
int netmgr_set_dhcp(void) { return 0; }
int netmgr_get_ip_address(void) { return 0; }
int netmgr_enable_proxy(const char* proxy_addr) { return (proxy_addr) ? 0 : -1; }
int netmgr_disable_proxy(void) { return 0; }
