#include <services/neural_activity_monitor.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ACTIVITY_EVENTS 1024
#define MAX_FILTERS 32

static activity_event_t g_events[MAX_ACTIVITY_EVENTS] = {0};
static uint32_t g_event_count = 0;
static activity_stats_t g_stats = {0};
static uint8_t g_enabled = 1;
static uint8_t g_realtime_viz = 1;
static uint32_t g_update_interval_ms = 100;

typedef struct {
    activity_type_t type;
    char pattern[128];
} activity_filter_t;

static activity_filter_t g_filters[MAX_FILTERS] = {0};
static uint32_t g_filter_count = 0;

static void record_event(activity_type_t type, uint32_t pid, const char *resource, uint64_t bytes) {
    if (g_event_count >= MAX_ACTIVITY_EVENTS) {
        // Shift left
        memmove(&g_events[0], &g_events[1], sizeof(activity_event_t) * (MAX_ACTIVITY_EVENTS - 1));
        g_event_count = MAX_ACTIVITY_EVENTS - 1;
    }
    activity_event_t *ev = &g_events[g_event_count++];
    ev->type = type;
    ev->pid = pid;
    ev->timestamp = (uint64_t)time(NULL);
    strncpy(ev->resource, resource ? resource : "unknown", 127);
    ev->bytes_transferred = bytes;

    g_stats.total_events++;
    switch (type) {
        case ACTIVITY_FILE_ACCESS: g_stats.file_accesses++; break;
        case ACTIVITY_MEMORY: g_stats.memory_ops++; break;
        case ACTIVITY_NETWORK: g_stats.network_packets++; break;
        case ACTIVITY_PROCESS: g_stats.process_spawns++; break;
        case ACTIVITY_DISK_IO: g_stats.disk_ios++; break;
        default: break;
    }
}

int neural_init(void) {
    g_event_count = 0;
    memset(&g_stats, 0, sizeof(g_stats));
    g_enabled = 1;
    g_realtime_viz = 1;
    g_update_interval_ms = 100;
    g_filter_count = 0;

    // Seed some initial live system events for simulation
    record_event(ACTIVITY_PROCESS, 1, "kernel_main", 0);
    record_event(ACTIVITY_MEMORY, 1, "mmgr_paging", 4096 * 32);
    record_event(ACTIVITY_FILE_ACCESS, 10, "/system/etc/aegis.conf", 1024);
    record_event(ACTIVITY_NETWORK, 42, "socket:tcp://127.0.0.1:8080", 512);
    record_event(ACTIVITY_DISK_IO, 10, "/dev/nvme0n1p1", 8192);

    return 0;
}

int neural_enable(void) {
    g_enabled = 1;
    return 0;
}

int neural_disable(void) {
    g_enabled = 0;
    return 0;
}

int neural_get_live_activity(activity_event_t *events, uint32_t *count, uint32_t max_events) {
    if (!events || !count) return -1;
    uint32_t to_copy = (g_event_count < max_events) ? g_event_count : max_events;
    for (uint32_t i = 0; i < to_copy; i++) {
        events[i] = g_events[g_event_count - to_copy + i];
    }
    *count = to_copy;
    return 0;
}

int neural_get_activity_for_pid(uint32_t pid, activity_event_t *events, uint32_t *count, uint32_t max_events) {
    if (!events || !count) return -1;
    uint32_t matched = 0;
    for (uint32_t i = 0; i < g_event_count && matched < max_events; i++) {
        if (g_events[i].pid == pid) {
            events[matched++] = g_events[i];
        }
    }
    *count = matched;
    return 0;
}

int neural_render_activity_graph(void) {
    return 0;
}

int neural_export_activity_visualization(const char *output_file) {
    if (!output_file) return -1;
    FILE *fp = fopen(output_file, "w");
    if (!fp) return -1;
    fprintf(fp, "=== AegisOS Neural Activity Report ===\n");
    fprintf(fp, "Total Events: %u\n", g_stats.total_events);
    fprintf(fp, "File Accesses: %u\n", g_stats.file_accesses);
    fprintf(fp, "Memory Ops: %u\n", g_stats.memory_ops);
    fprintf(fp, "Network Packets: %u\n", g_stats.network_packets);
    fprintf(fp, "Process Spawns: %u\n", g_stats.process_spawns);
    fprintf(fp, "Disk I/O: %u\n", g_stats.disk_ios);
    fclose(fp);
    return 0;
}

int neural_get_stats(activity_stats_t *stats) {
    if (!stats) return -1;
    *stats = g_stats;
    return 0;
}

int neural_clear_history(void) {
    g_event_count = 0;
    memset(&g_stats, 0, sizeof(g_stats));
    return 0;
}

int neural_set_update_interval(uint32_t interval_ms) {
    if (interval_ms < 10) return -1;
    g_update_interval_ms = interval_ms;
    return 0;
}

int neural_enable_real_time_visualization(void) {
    g_realtime_viz = 1;
    return 0;
}

int neural_disable_real_time_visualization(void) {
    g_realtime_viz = 0;
    return 0;
}

int neural_detect_anomaly(activity_type_t type, const char *pattern, uint8_t *detected) {
    if (!detected) return -1;
    *detected = 0;
    if (!pattern) return -1;

    // Anomaly detection heuristic:
    // If scanning for ransomware or high entropy burst
    if (strstr(pattern, "ransomware") || strstr(pattern, "crypt") || strstr(pattern, "burst")) {
        // Check if file accesses or memory ops are high
        if (g_stats.file_accesses > 50 || g_stats.memory_ops > 100) {
            *detected = 1;
            return 0;
        }
    }

    for (uint32_t i = 0; i < g_event_count; i++) {
        if (g_events[i].type == type && strstr(g_events[i].resource, pattern) != NULL) {
            *detected = 1;
            break;
        }
    }
    return 0;
}

int neural_register_activity_filter(activity_type_t type, const char *filter) {
    if (!filter || g_filter_count >= MAX_FILTERS) return -1;
    g_filters[g_filter_count].type = type;
    strncpy(g_filters[g_filter_count].pattern, filter, 127);
    g_filter_count++;
    return 0;
}
