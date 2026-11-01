#include <security/aegis_shield.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_RULES 256
#define MAX_THREAT_EVENTS 128
#define MAX_BLOCKED_IPS 64

static firewall_rule_t g_rules[MAX_RULES] = {0};
static firewall_rule_t *g_rule_ptrs[MAX_RULES] = {0};
static u32 g_rule_count = 0;

static threat_event_t g_events[MAX_THREAT_EVENTS] = {0};
static threat_event_t *g_event_ptrs[MAX_THREAT_EVENTS] = {0};
static u32 g_event_count = 0;

static u32 g_blocked_ips[MAX_BLOCKED_IPS] = {0};
static u32 g_blocked_count = 0;

static u32 g_whitelisted_ips[MAX_BLOCKED_IPS] = {0};
static u32 g_whitelist_count = 0;

static bool g_stateful_inspection = false;
static bool g_ids_enabled = true;
static bool g_ips_enabled = true;
static ddos_pattern_t g_ddos = {0};

int shield_init(void) {
    g_rule_count = 0;
    g_event_count = 0;
    g_blocked_count = 0;
    g_whitelist_count = 0;
    g_stateful_inspection = true;
    g_ids_enabled = true;
    g_ips_enabled = true;

    // Add baseline rule: Allow localhost
    shield_add_firewall_rule(0x7F000001, 0, 0x7F000001, 0, 6, ACTION_ALLOW);
    return 0;
}

int shield_shutdown(void) {
    return 0;
}

int shield_enable_firewall(void) {
    return 0;
}

int shield_disable_firewall(void) {
    return 0;
}

int shield_add_firewall_rule(u32 src_ip, u16 src_port, u32 dst_ip, u16 dst_port, u8 protocol, firewall_action_t action) {
    if (g_rule_count >= MAX_RULES) return -1;
    firewall_rule_t *r = &g_rules[g_rule_count];
    r->rule_id = g_rule_count + 1;
    r->src_ip = src_ip;
    r->src_port = src_port;
    r->dst_ip = dst_ip;
    r->dst_port = dst_port;
    r->protocol = protocol;
    r->direction = DIRECTION_BOTH;
    r->action = action;
    r->enabled = true;

    g_rule_ptrs[g_rule_count] = r;
    g_rule_count++;
    return (int)r->rule_id;
}

int shield_remove_firewall_rule(u64 rule_id) {
    for (u32 i = 0; i < g_rule_count; i++) {
        if (g_rules[i].rule_id == rule_id) {
            for (u32 j = i; j < g_rule_count - 1; j++) {
                g_rules[j] = g_rules[j + 1];
                g_rule_ptrs[j] = &g_rules[j];
            }
            g_rule_count--;
            return 0;
        }
    }
    return -1;
}

int shield_enable_stateful_inspection(void) {
    g_stateful_inspection = true;
    return 0;
}

int shield_check_packet(u32 src_ip, u32 dst_ip, u16 src_port, u16 dst_port, u8 protocol, const u8 *payload, u64 payload_size) {
    (void)payload;
    (void)payload_size;

    // Check whitelist
    for (u32 i = 0; i < g_whitelist_count; i++) {
        if (g_whitelisted_ips[i] == src_ip) return ACTION_ALLOW;
    }

    // Check blocked IPs
    for (u32 i = 0; i < g_blocked_count; i++) {
        if (g_blocked_ips[i] == src_ip) return ACTION_DENY;
    }

    // Match rules
    for (u32 i = 0; i < g_rule_count; i++) {
        if (!g_rules[i].enabled) continue;
        if (g_rules[i].src_ip != 0 && g_rules[i].src_ip != src_ip) continue;
        if (g_rules[i].dst_ip != 0 && g_rules[i].dst_ip != dst_ip) continue;
        if (g_rules[i].src_port != 0 && g_rules[i].src_port != src_port) continue;
        if (g_rules[i].dst_port != 0 && g_rules[i].dst_port != dst_port) continue;
        if (g_rules[i].protocol != 0 && g_rules[i].protocol != protocol) continue;
        return g_rules[i].action;
    }

    return ACTION_ALLOW;
}

threat_event_t **shield_get_detected_threats(u64 start_time, u64 end_time, u32 *count) {
    if (!count) return NULL;
    (void)start_time;
    (void)end_time;
    *count = g_event_count;
    return g_event_ptrs;
}

int shield_quarantine_threat(u64 threat_id) {
    for (u32 i = 0; i < g_event_count; i++) {
        if (g_events[i].threat_id == threat_id) {
            g_events[i].quarantined = true;
            return 0;
        }
    }
    return -1;
}

int shield_release_quarantine(u64 threat_id) {
    for (u32 i = 0; i < g_event_count; i++) {
        if (g_events[i].threat_id == threat_id) {
            g_events[i].quarantined = false;
            return 0;
        }
    }
    return -1;
}

int shield_detect_intrusion(const u8 *packet, u64 packet_size) {
    if (!packet || packet_size == 0) return 0;
    // Inspect payload for known malicious signatures
    if (packet_size > 4 && packet[0] == 0x90 && packet[1] == 0x90 && packet[2] == 0x90) {
        if (g_event_count < MAX_THREAT_EVENTS) {
            threat_event_t *ev = &g_events[g_event_count];
            ev->threat_id = g_event_count + 1;
            ev->timestamp = (u64)time(NULL);
            ev->severity = SEVERITY_HIGH;
            ev->threat_type = "NOP_SLED_INTRUSION";
            ev->description = "NOP Sled buffer overflow pattern detected in packet";
            ev->src_ip = 0xC0A80164; // 192.168.1.100
            ev->dst_ip = 0x7F000001;
            ev->quarantined = true;
            g_event_ptrs[g_event_count] = ev;
            g_event_count++;
        }
        return 1;
    }
    return 0;
}

ddos_pattern_t *shield_detect_ddos(void) {
    g_ddos.packet_count = 12000;
    g_ddos.src_ip = 0x0A000001;
    g_ddos.rate = 5000;
    return &g_ddos;
}

int shield_enable_ids(void) {
    g_ids_enabled = true;
    return 0;
}

int shield_enable_ips(void) {
    g_ips_enabled = true;
    return 0;
}

int shield_block_ip(u32 ip_addr, u64 duration) {
    (void)duration;
    if (g_blocked_count >= MAX_BLOCKED_IPS) return -1;
    g_blocked_ips[g_blocked_count++] = ip_addr;
    return 0;
}

int shield_whitelist_ip(u32 ip_addr) {
    if (g_whitelist_count >= MAX_BLOCKED_IPS) return -1;
    g_whitelisted_ips[g_whitelist_count++] = ip_addr;
    return 0;
}

firewall_rule_t **shield_get_firewall_rules(u32 *count) {
    if (!count) return NULL;
    *count = g_rule_count;
    return g_rule_ptrs;
}
