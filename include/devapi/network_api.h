#ifndef AEGIS_DEVAPI_NETWORK_H
#define AEGIS_DEVAPI_NETWORK_H

#include <kernel/types.h>

typedef enum {
    AEGIS_SOCKET_AF_INET = 2,
    AEGIS_SOCKET_AF_INET6 = 10,
    AEGIS_SOCKET_AF_UNIX = 1,
} aegis_socket_family_t;

typedef enum {
    AEGIS_SOCKET_STREAM = 1,
    AEGIS_SOCKET_DGRAM = 2,
    AEGIS_SOCKET_RAW = 3,
} aegis_socket_type_t;

typedef enum {
    AEGIS_SOCKET_IPPROTO_TCP = 6,
    AEGIS_SOCKET_IPPROTO_UDP = 17,
    AEGIS_SOCKET_IPPROTO_SCTP = 132,
} aegis_socket_protocol_t;

typedef enum {
    AEGIS_TLS_VERSION_1_2 = 0x0303,
    AEGIS_TLS_VERSION_1_3 = 0x0303,
} aegis_tls_version_t;

typedef struct {
    char address[256];
    uint16_t port;
} aegis_socket_address_t;

typedef struct {
    uint8_t data[4];
} aegis_ipv4_t;

typedef struct {
    uint8_t data[16];
} aegis_ipv6_t;

typedef struct {
    uint8_t type;
    union {
        aegis_ipv4_t ipv4;
        aegis_ipv6_t ipv6;
        char unix_path[108];
    } addr;
} aegis_ip_address_t;

typedef struct aegis_socket aegis_socket_t;
typedef struct aegis_sync_task aegis_sync_task_t;

typedef void (*aegis_socket_callback_fn)(aegis_socket_t *socket, uint32_t event, void *ctx);
typedef void (*aegis_sync_task_callback_fn)(int status, void *ctx);

int aegis_socket_create(aegis_socket_family_t family, aegis_socket_type_t type,
                        aegis_socket_protocol_t protocol, aegis_socket_t **socket);
int aegis_socket_destroy(aegis_socket_t *socket);

int aegis_socket_bind(aegis_socket_t *socket, const aegis_socket_address_t *addr);
int aegis_socket_listen(aegis_socket_t *socket, uint32_t backlog);
int aegis_socket_accept(aegis_socket_t *socket, aegis_socket_t **client, 
                        aegis_socket_address_t *client_addr);
int aegis_socket_connect(aegis_socket_t *socket, const aegis_socket_address_t *addr, uint32_t timeout_ms);

int aegis_socket_send(aegis_socket_t *socket, const void *data, uint32_t size, uint32_t *bytes_sent);
int aegis_socket_send_to(aegis_socket_t *socket, const void *data, uint32_t size,
                         const aegis_socket_address_t *addr, uint32_t *bytes_sent);
int aegis_socket_receive(aegis_socket_t *socket, void *buffer, uint32_t size, uint32_t *bytes_received);
int aegis_socket_receive_from(aegis_socket_t *socket, void *buffer, uint32_t size,
                              aegis_socket_address_t *addr, uint32_t *bytes_received);

int aegis_socket_send_async(aegis_socket_t *socket, const void *data, uint32_t size,
                            aegis_socket_callback_fn callback, void *ctx);
int aegis_socket_receive_async(aegis_socket_t *socket, void *buffer, uint32_t size,
                               aegis_socket_callback_fn callback, void *ctx);

int aegis_socket_set_blocking(aegis_socket_t *socket, uint8_t blocking);
int aegis_socket_set_timeout(aegis_socket_t *socket, uint32_t timeout_ms);
int aegis_socket_set_buffer_size(aegis_socket_t *socket, uint32_t send_size, uint32_t recv_size);

int aegis_socket_enable_encryption(aegis_socket_t *socket, aegis_tls_version_t tls_version);
int aegis_socket_set_certificate(aegis_socket_t *socket, const char *cert_path, const char *key_path);
int aegis_socket_set_ca_certificate(aegis_socket_t *socket, const char *ca_cert_path);
int aegis_socket_verify_certificate(aegis_socket_t *socket);

int aegis_socket_shutdown(aegis_socket_t *socket, uint8_t how);
int aegis_socket_close(aegis_socket_t *socket);

int aegis_socket_set_option(aegis_socket_t *socket, uint32_t level, uint32_t optname, 
                            const void *optval, uint32_t optlen);
int aegis_socket_get_option(aegis_socket_t *socket, uint32_t level, uint32_t optname,
                            void *optval, uint32_t *optlen);

int aegis_socket_register_callback(aegis_socket_t *socket, uint32_t event_mask,
                                   aegis_socket_callback_fn callback, void *ctx);
int aegis_socket_unregister_callback(aegis_socket_t *socket, aegis_socket_callback_fn callback);

int aegis_dns_resolve(const char *hostname, aegis_ip_address_t *address);
int aegis_dns_reverse_lookup(const aegis_ip_address_t *address, char *hostname, uint32_t max_len);

int aegis_sync_task_create(aegis_sync_task_t **task);
int aegis_sync_task_destroy(aegis_sync_task_t *task);
int aegis_sync_task_set_destination(aegis_sync_task_t *task, const char *destination_url);
int aegis_sync_task_set_interval(aegis_sync_task_t *task, uint32_t interval_seconds);
int aegis_sync_task_set_data(aegis_sync_task_t *task, const void *data, uint32_t size);
int aegis_sync_task_start(aegis_sync_task_t *task, aegis_sync_task_callback_fn callback, void *ctx);
int aegis_sync_task_stop(aegis_sync_task_t *task);
int aegis_sync_task_is_running(aegis_sync_task_t *task);

int aegis_http_get(const char *url, char *response, uint32_t max_response_len, uint32_t timeout_ms);
int aegis_http_post(const char *url, const char *body, char *response, 
                    uint32_t max_response_len, uint32_t timeout_ms);
int aegis_http_put(const char *url, const char *body, char *response, 
                   uint32_t max_response_len, uint32_t timeout_ms);
int aegis_http_delete(const char *url, uint32_t timeout_ms);

int aegis_websocket_connect(const char *url, aegis_socket_t **socket);
int aegis_websocket_send_message(aegis_socket_t *socket, const char *message);
int aegis_websocket_receive_message(aegis_socket_t *socket, char *message, uint32_t max_len);
int aegis_websocket_close(aegis_socket_t *socket);

int aegis_network_get_interfaces(char *interfaces, uint32_t max_len);
int aegis_network_get_ipv4_address(const char *interface, char *address, uint32_t max_len);
int aegis_network_get_ipv6_address(const char *interface, char *address, uint32_t max_len);

#endif
