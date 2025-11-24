#ifndef AEGIS_SERVICES_NEURAL_ACTIVITY_MONITOR_H
#define AEGIS_SERVICES_NEURAL_ACTIVITY_MONITOR_H

#include <kernel/types.h>

typedef enum {
    ACTIVITY_FILE_ACCESS = 1,
    ACTIVITY_MEMORY = 2,
    ACTIVITY_NETWORK = 3,
    ACTIVITY_PROCESS = 4,
    ACTIVITY_DISK_IO = 5,
} activity_type_t;

typedef struct {
    activity_type_t type;
    uint32_t pid;
    uint64_t timestamp;
    char resource[128];
    uint64_t bytes_transferred;
} activity_event_t;

typedef struct {
    uint32_t total_events;
    uint32_t file_accesses;
    uint32_t memory_ops;
    uint32_t network_packets;
    uint32_t process_spawns;
    uint32_t disk_ios;
} activity_stats_t;

int neural_init(void);
int neural_enable(void);
int neural_disable(void);

int neural_get_live_activity(activity_event_t *events, uint32_t *count, uint32_t max_events);
int neural_get_activity_for_pid(uint32_t pid, activity_event_t *events, uint32_t *count, uint32_t max_events);

int neural_render_activity_graph(void);
int neural_export_activity_visualization(const char *output_file);

int neural_get_stats(activity_stats_t *stats);
int neural_clear_history(void);

int neural_set_update_interval(uint32_t interval_ms);
int neural_enable_real_time_visualization(void);
int neural_disable_real_time_visualization(void);

int neural_detect_anomaly(activity_type_t type, const char *pattern, uint8_t *detected);
int neural_register_activity_filter(activity_type_t type, const char *filter);

#endif
