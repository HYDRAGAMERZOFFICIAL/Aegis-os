#ifndef AEGIS_NETWORK_MANAGER_H
#define AEGIS_NETWORK_MANAGER_H

#include <kernel/types.h>

typedef enum {
    CONN_TYPE_ETHERNET,
    CONN_TYPE_WIFI,
    CONN_TYPE_CELLULAR,
    CONN_TYPE_VPN,
    CONN_TYPE_BLUETOOTH
} connection_type_t;

typedef enum {
    CONN_STATE_DISCONNECTED,
    CONN_STATE_CONNECTING,
    CONN_STATE_CONNECTED,
    CONN_STATE_DISCONNECTING,
    CONN_STATE_FAILED
} connection_state_t;

typedef struct {
    u64 connection_id;
    connection_type_t type;
    connection_state_t state;
    u32 priority;
    u32 signal_strength;
    u64 bytes_sent, bytes_received;
    bool is_encrypted;
} connection_t;

typedef struct {
    char *ssid;
    u32 signal_strength;
    bool is_open;
    u32 security_type;
} wifi_network_t;

int network_mgr_init(void);
connection_t **network_mgr_get_connections(u32 *count);
int network_mgr_connect(connection_type_t type, const char *name);
int network_mgr_disconnect(u64 connection_id);
int network_mgr_set_priority(u64 connection_id, u32 priority);
wifi_network_t **network_mgr_scan_wifi(u32 *count);
int network_mgr_connect_wifi(const char *ssid, const char *password);
int network_mgr_enable_vpn(const char *vpn_name, const u8 *config, u64 config_size);
int network_mgr_disable_vpn(const char *vpn_name);
int network_mgr_enable_doh(const char *doh_server);
connection_state_t network_mgr_get_state(u64 connection_id);
int network_mgr_set_dns(const char *primary, const char *secondary);
int network_mgr_get_ip_address(u64 connection_id, char *ip_buf, u64 buf_size);

#endif
