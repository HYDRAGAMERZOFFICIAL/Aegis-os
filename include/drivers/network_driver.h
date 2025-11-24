#ifndef AEGIS_DRIVERS_NETWORK_H
#define AEGIS_DRIVERS_NETWORK_H

#include <kernel/types.h>

typedef enum {
    NETWORK_TYPE_ETHERNET = 1,
    NETWORK_TYPE_WIFI = 2,
    NETWORK_TYPE_BLUETOOTH = 3,
    NETWORK_TYPE_CELLULAR = 4,
    NETWORK_TYPE_VPNBRIDGE = 5,
} network_interface_type_t;

typedef enum {
    NETWORK_STATUS_DOWN = 0,
    NETWORK_STATUS_UP = 1,
    NETWORK_STATUS_CONNECTING = 2,
    NETWORK_STATUS_SCANNING = 3,
} network_status_t;

typedef struct {
    char name[32];
    network_interface_type_t type;
    uint8_t mac_address[6];
    uint32_t mtu;
    uint64_t rx_packets;
    uint64_t tx_packets;
    uint64_t rx_errors;
    uint64_t tx_errors;
} network_interface_info_t;

typedef struct {
    uint8_t ssid[32];
    uint8_t ssid_len;
    uint8_t bssid[6];
    int8_t signal_strength;
    uint8_t channel;
    uint8_t security_type;
} wifi_network_t;

typedef struct {
    uint8_t *data;
    uint32_t size;
    uint32_t flags;
    uint64_t timestamp_us;
} network_packet_t;

typedef struct network_interface network_interface_t;

typedef void (*network_rx_callback_fn)(network_interface_t *iface, network_packet_t *packet, void *ctx);
typedef void (*network_tx_callback_fn)(network_interface_t *iface, int status, void *ctx);
typedef int (*network_threat_detector_fn)(network_interface_t *iface, network_packet_t *packet, void *ctx);

int network_driver_init(void);
int network_driver_shutdown(void);

int network_enumerate_interfaces(network_interface_info_t *interfaces, uint32_t *count, uint32_t max_interfaces);
network_interface_t *network_open_interface(uint32_t interface_id);
int network_close_interface(network_interface_t *iface);

int network_interface_up(network_interface_t *iface);
int network_interface_down(network_interface_t *iface);
network_status_t network_get_status(network_interface_t *iface);

int network_transmit_packet(network_interface_t *iface, network_packet_t *packet);
int network_transmit_packet_async(network_interface_t *iface, network_packet_t *packet,
                                  network_tx_callback_fn callback, void *ctx);

int network_register_rx_handler(network_interface_t *iface, network_rx_callback_fn handler, void *ctx);
int network_unregister_rx_handler(network_interface_t *iface, network_rx_callback_fn handler);

int network_get_interface_info(network_interface_t *iface, network_interface_info_t *info);
int network_set_mtu(network_interface_t *iface, uint32_t mtu);

int network_wifi_scan(network_interface_t *iface, wifi_network_t *networks, uint32_t *count, uint32_t max_networks);
int network_wifi_connect(network_interface_t *iface, const char *ssid, const char *password, uint32_t security_type);
int network_wifi_disconnect(network_interface_t *iface);
int network_wifi_get_signal_strength(network_interface_t *iface);

int network_bluetooth_scan(network_interface_t *iface, char *device_list, uint32_t max_len);
int network_bluetooth_pair(network_interface_t *iface, const char *device_addr);
int network_bluetooth_unpair(network_interface_t *iface, const char *device_addr);

int network_register_threat_detector(network_interface_t *iface, network_threat_detector_fn detector, void *ctx);
int network_enable_anomaly_detection(network_interface_t *iface);
int network_disable_anomaly_detection(network_interface_t *iface);

int network_enable_intrusion_prevention(network_interface_t *iface);
int network_disable_intrusion_prevention(network_interface_t *iface);

int network_get_statistics(network_interface_t *iface, char *stats, uint32_t max_len);
int network_reset_statistics(network_interface_t *iface);

uint32_t network_get_rx_queue_size(network_interface_t *iface);
int network_flush_rx_queue(network_interface_t *iface);

int network_set_power_mode(network_interface_t *iface, uint8_t power_state);
int network_get_link_speed(network_interface_t *iface, uint32_t *speed_mbps);

#endif
