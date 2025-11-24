#ifndef AEGIS_KERNEL_NETWORK_H
#define AEGIS_KERNEL_NETWORK_H

#include <kernel/types.h>

typedef enum {
    NET_PROTO_ETHERNET,
    NET_PROTO_IP,
    NET_PROTO_TCP,
    NET_PROTO_UDP,
    NET_PROTO_ICMP
} net_protocol_t;

typedef struct {
    u8 dst_mac[6];
    u8 src_mac[6];
    u16 ethertype;
} eth_header_t;

typedef struct {
    u8 version_ihl;
    u8 dscp_ecn;
    u16 total_length;
    u16 identification;
    u16 flags_frag_offset;
    u8 ttl;
    u8 protocol;
    u16 checksum;
    u32 src_ip;
    u32 dst_ip;
} ipv4_header_t;

typedef struct {
    u16 src_port;
    u16 dst_port;
    u32 sequence_num;
    u32 ack_num;
    u16 flags;
    u16 window_size;
    u16 checksum;
    u16 urgent_ptr;
} tcp_header_t;

typedef struct {
    u16 src_port;
    u16 dst_port;
    u16 length;
    u16 checksum;
} udp_header_t;

typedef struct {
    u64 src_ip;
    u64 dst_ip;
    u16 src_port;
    u16 dst_port;
    enum {
        SOCKET_STATE_CLOSED,
        SOCKET_STATE_LISTEN,
        SOCKET_STATE_ESTABLISHED,
        SOCKET_STATE_TIME_WAIT
    } state;
    bool encrypted;
    void *encryption_key;
} socket_t;

typedef struct {
    u8 *data;
    u64 size;
    net_protocol_t protocol;
    bool encrypted;
} packet_t;

int network_init(void);
int network_register_handler(net_protocol_t proto, handler_t handler);
int network_send_packet(packet_t *pkt, u32 device);
packet_t *network_receive_packet(u32 device, u64 timeout);
socket_t *network_create_socket(u16 family, u16 type);
int network_connect(socket_t *sock, u64 dst_ip, u16 dst_port);
int network_bind(socket_t *sock, u16 port);
int network_listen(socket_t *sock, u32 backlog);
socket_t *network_accept(socket_t *sock);
int network_send(socket_t *sock, const void *data, u64 size);
int network_receive(socket_t *sock, void *buffer, u64 max_size);
int network_close(socket_t *sock);
int network_enable_encryption(socket_t *sock, const u8 *key, u64 key_size);
int network_enable_firewall(void);
int network_add_firewall_rule(const char *rule);
int network_enable_encrypted_sockets(void);

#endif
