#ifndef AEGIS_PERF_OPTIMIZE_H
#define AEGIS_PERF_OPTIMIZE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PERF_MAX_SAMPLES 10000
#define PERF_MAX_EVENTS 256
#define PERF_MAX_CALLSTACKS 100

typedef enum {
    PERF_EVENT_CONTEXT_SWITCH = 1,
    PERF_EVENT_CACHE_MISS = 2,
    PERF_EVENT_MEMORY_ALLOC = 3,
    PERF_EVENT_IPC_MESSAGE = 4,
    PERF_EVENT_SYSCALL = 5,
    PERF_EVENT_PAGE_FAULT = 6,
    PERF_EVENT_INTERRUPT = 7,
    PERF_EVENT_FUNCTION_ENTRY = 8,
    PERF_EVENT_FUNCTION_EXIT = 9,
    PERF_EVENT_LOCK_CONTENTION = 10
} perf_event_type_t;

typedef struct {
    uint64_t timestamp;
    uint32_t cpu_id;
    uint32_t pid;
    uint32_t tid;
    perf_event_type_t event_type;
    uint64_t event_value;
    uint64_t duration_ns;
    uint32_t call_stack[16];
    uint32_t call_depth;
} perf_sample_t;

typedef struct {
    perf_event_type_t event_type;
    uint64_t count;
    uint64_t total_duration_ns;
    uint64_t min_duration_ns;
    uint64_t max_duration_ns;
    uint64_t avg_duration_ns;
    uint32_t frequency;
} perf_event_stats_t;

typedef struct {
    perf_sample_t samples[PERF_MAX_SAMPLES];
    size_t sample_count;
    uint64_t collection_start;
    uint64_t collection_end;
    bool is_active;
} perf_buffer_t;

typedef struct {
    perf_event_stats_t event_stats[PERF_MAX_EVENTS];
    size_t event_count;
    uint64_t total_context_switches;
    uint64_t total_page_faults;
    uint64_t total_cache_misses;
    uint64_t peak_memory_usage;
} perf_statistics_t;

typedef struct {
    uint64_t hotspot_function;
    uint64_t call_count;
    uint64_t total_duration_ns;
    uint64_t self_duration_ns;
} perf_hotspot_t;

typedef struct {
    perf_hotspot_t hotspots[100];
    size_t hotspot_count;
} perf_hotspot_analysis_t;

perf_buffer_t *perf_create_buffer(void);
void perf_start_collection(perf_buffer_t *buffer);
void perf_stop_collection(perf_buffer_t *buffer);
void perf_record_sample(perf_buffer_t *buffer, const perf_sample_t *sample);
void perf_clear_buffer(perf_buffer_t *buffer);

perf_statistics_t *perf_analyze_buffer(perf_buffer_t *buffer);
perf_hotspot_analysis_t *perf_find_hotspots(perf_buffer_t *buffer);
uint64_t perf_get_context_switch_overhead(perf_buffer_t *buffer);
uint64_t perf_get_syscall_latency(perf_buffer_t *buffer);
uint64_t perf_get_ipc_latency(perf_buffer_t *buffer);

bool perf_optimize_cpu_cache(void);
bool perf_optimize_memory_allocator(void);
bool perf_optimize_scheduler(void);
bool perf_optimize_ipc_bus(void);

void perf_report_statistics(perf_statistics_t *stats);
void perf_report_hotspots(perf_hotspot_analysis_t *hotspots);
void perf_free_statistics(perf_statistics_t *stats);
void perf_free_hotspot_analysis(perf_hotspot_analysis_t *hotspots);
void perf_free_buffer(perf_buffer_t *buffer);

#endif
