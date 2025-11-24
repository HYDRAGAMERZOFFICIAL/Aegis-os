#ifndef AEGIS_SHIELD_H
#define AEGIS_SHIELD_H

#include <kernel/types.h>

typedef enum {
    DIRECTION_INBOUND,
    DIRECTION_OUTBOUND,
    DIRECTION_BOTH
} direction_t;

typedef enum {
    ACTION_ALLOW,
    ACTION_DENY,
    ACTION_QUARANTINE,
    ACTION_LOG_ONLY
} firewall_action_t;

typedef enum {
    SEVERITY_LOW,
    SEVERITY_MEDIUM,
    SEVERITY_HIGH,
    SEVERITY_CRITICAL
} threat_severity_t;

typedef struct {
    u64 rule_id;
    u32 src_ip, dst_ip;
    u16 src_port, dst_port;
    u8 protocol;
    direction_t direction;
    firewall_action_t action;
    bool enabled;
} firewall_rule_t;

typedef struct {
    u64 threat_id;
    u64 timestamp;
    threat_severity_t severity;
    const char *threat_type;
    const char *description;
    u32 src_ip, dst_ip;
    bool quarantined;
} threat_event_t;

typedef struct {
    u64 packet_count;
    u32 src_ip;
    u64 rate;
} ddos_pattern_t;

int shield_init(void);
int shield_add_firewall_rule(u32 src_ip, u16 src_port, u32 dst_ip, u16 dst_port, u8 protocol, firewall_action_t action);
int shield_remove_firewall_rule(u64 rule_id);
int shield_enable_stateful_inspection(void);
int shield_check_packet(u32 src_ip, u32 dst_ip, u16 src_port, u16 dst_port, u8 protocol, const u8 *payload, u64 payload_size);
threat_event_t **shield_get_detected_threats(u64 start_time, u64 end_time, u32 *count);
int shield_quarantine_threat(u64 threat_id);
int shield_release_quarantine(u64 threat_id);
int shield_detect_intrusion(const u8 *packet, u64 packet_size);
ddos_pattern_t *shield_detect_ddos(void);
int shield_enable_ids(void);
int shield_enable_ips(void);
int shield_block_ip(u32 ip_addr, u64 duration);
int shield_whitelist_ip(u32 ip_addr);
firewall_rule_t **shield_get_firewall_rules(u32 *count);

#endif
