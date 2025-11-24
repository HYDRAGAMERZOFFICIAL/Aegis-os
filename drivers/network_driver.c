#include <drivers/network_driver.h>
#include <string.h>

int network_driver_init(void) { return 0; }
int network_driver_shutdown(void) { return 0; }

int network_enumerate_interfaces(network_interface_info_t *interfaces, uint32_t *count, uint32_t max_interfaces) { return 0; }
network_interface_t *network_open_interface(uint32_t interface_id) { return 0; }
int network_close_interface(network_interface_t *iface) { return 0; }

int network_interface_up(network_interface_t *iface) { return 0; }
int network_interface_down(network_interface_t *iface) { return 0; }
network_status_t network_get_status(network_interface_t *iface) { return NETWORK_STATUS_DOWN; }

int network_transmit_packet(network_interface_t *iface, network_packet_t *packet) { return 0; }
int network_transmit_packet_async(network_interface_t *iface, network_packet_t *packet,
                                  network_tx_callback_fn callback, void *ctx) { return 0; }

int network_register_rx_handler(network_interface_t *iface, network_rx_callback_fn handler, void *ctx) { return 0; }
int network_unregister_rx_handler(network_interface_t *iface, network_rx_callback_fn handler) { return 0; }

int network_get_interface_info(network_interface_t *iface, network_interface_info_t *info) { return 0; }
int network_set_mtu(network_interface_t *iface, uint32_t mtu) { return 0; }

int network_wifi_scan(network_interface_t *iface, wifi_network_t *networks, uint32_t *count, uint32_t max_networks) { return 0; }
int network_wifi_connect(network_interface_t *iface, const char *ssid, const char *password, uint32_t security_type) { return 0; }
int network_wifi_disconnect(network_interface_t *iface) { return 0; }
int network_wifi_get_signal_strength(network_interface_t *iface) { return 0; }

int network_bluetooth_scan(network_interface_t *iface, char *device_list, uint32_t max_len) { return 0; }
int network_bluetooth_pair(network_interface_t *iface, const char *device_addr) { return 0; }
int network_bluetooth_unpair(network_interface_t *iface, const char *device_addr) { return 0; }

int network_register_threat_detector(network_interface_t *iface, network_threat_detector_fn detector, void *ctx) { return 0; }
int network_enable_anomaly_detection(network_interface_t *iface) { return 0; }
int network_disable_anomaly_detection(network_interface_t *iface) { return 0; }

int network_enable_intrusion_prevention(network_interface_t *iface) { return 0; }
int network_disable_intrusion_prevention(network_interface_t *iface) { return 0; }

int network_get_statistics(network_interface_t *iface, char *stats, uint32_t max_len) { return 0; }
int network_reset_statistics(network_interface_t *iface) { return 0; }

uint32_t network_get_rx_queue_size(network_interface_t *iface) { return 0; }
int network_flush_rx_queue(network_interface_t *iface) { return 0; }

int network_set_power_mode(network_interface_t *iface, uint8_t power_state) { return 0; }
int network_get_link_speed(network_interface_t *iface, uint32_t *speed_mbps) { return 0; }
