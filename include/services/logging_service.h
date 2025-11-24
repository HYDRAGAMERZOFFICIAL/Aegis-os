#ifndef AEGIS_LOGGING_SERVICE_H
#define AEGIS_LOGGING_SERVICE_H

#include <kernel/types.h>

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL
} log_level_t;

typedef struct {
    u64 timestamp;
    u64 source_pid;
    log_level_t level;
    const char *component;
    const char *message;
    u8 *data;
    u64 data_size;
} log_entry_t;

typedef struct {
    u64 total_entries;
    u64 debug_count;
    u64 info_count;
    u64 warning_count;
    u64 error_count;
    u64 critical_count;
} log_stats_t;

int logging_init(void);
int logging_log(log_level_t level, const char *component, const char *fmt, ...);
log_entry_t **logging_get_entries(log_level_t min_level, u64 start_time, u64 end_time, u32 *count);
int logging_clear_entries(log_level_t level);
int logging_export_logs(const char *filename, u64 start_time, u64 end_time);
int logging_enable_real_time_monitoring(void);
log_stats_t logging_get_statistics(void);
int logging_set_telemetry_enabled(bool enabled);
int logging_get_telemetry_status(void);
int logging_register_sink(const char *sink_name, void (*sink_func)(log_entry_t *));
int logging_set_retention_policy(u64 max_size, u64 max_age);

#endif
