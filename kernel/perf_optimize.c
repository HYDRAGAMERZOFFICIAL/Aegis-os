#include "perf_optimize.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

perf_buffer_t *perf_create_buffer(void) {
    perf_buffer_t *buffer = malloc(sizeof(perf_buffer_t));
    if (!buffer) return NULL;
    
    memset(buffer, 0, sizeof(perf_buffer_t));
    buffer->collection_start = 0;
    buffer->is_active = false;
    
    return buffer;
}

void perf_start_collection(perf_buffer_t *buffer) {
    if (!buffer) return;
    
    buffer->is_active = true;
    buffer->sample_count = 0;
    buffer->collection_start = 0;
}

void perf_stop_collection(perf_buffer_t *buffer) {
    if (!buffer) return;
    
    buffer->is_active = false;
    buffer->collection_end = 0;
}

void perf_record_sample(perf_buffer_t *buffer, const perf_sample_t *sample) {
    if (!buffer || !sample || !buffer->is_active) return;
    
    if (buffer->sample_count >= PERF_MAX_SAMPLES) {
        return;
    }
    
    memcpy(&buffer->samples[buffer->sample_count], sample, sizeof(perf_sample_t));
    buffer->sample_count++;
}

void perf_clear_buffer(perf_buffer_t *buffer) {
    if (!buffer) return;
    
    memset(buffer->samples, 0, sizeof(perf_sample_t) * PERF_MAX_SAMPLES);
    buffer->sample_count = 0;
}

perf_statistics_t *perf_analyze_buffer(perf_buffer_t *buffer) {
    if (!buffer) return NULL;
    
    perf_statistics_t *stats = malloc(sizeof(perf_statistics_t));
    if (!stats) return NULL;
    
    memset(stats, 0, sizeof(perf_statistics_t));
    
    for (size_t i = 0; i < buffer->sample_count; i++) {
        perf_sample_t *sample = &buffer->samples[i];
        
        if (sample->event_type == PERF_EVENT_CONTEXT_SWITCH) {
            stats->total_context_switches++;
        } else if (sample->event_type == PERF_EVENT_PAGE_FAULT) {
            stats->total_page_faults++;
        } else if (sample->event_type == PERF_EVENT_CACHE_MISS) {
            stats->total_cache_misses++;
        }
        
        if (sample->event_type < PERF_MAX_EVENTS) {
            perf_event_stats_t *event_stat = &stats->event_stats[sample->event_type];
            
            if (event_stat->count == 0) {
                event_stat->min_duration_ns = sample->duration_ns;
                event_stat->max_duration_ns = sample->duration_ns;
            } else {
                if (sample->duration_ns < event_stat->min_duration_ns) {
                    event_stat->min_duration_ns = sample->duration_ns;
                }
                if (sample->duration_ns > event_stat->max_duration_ns) {
                    event_stat->max_duration_ns = sample->duration_ns;
                }
            }
            
            event_stat->count++;
            event_stat->total_duration_ns += sample->duration_ns;
            event_stat->event_type = sample->event_type;
            
            if (event_stat->count > 0) {
                event_stat->avg_duration_ns = event_stat->total_duration_ns / event_stat->count;
            }
        }
    }
    
    stats->event_count = PERF_MAX_EVENTS;
    return stats;
}

perf_hotspot_analysis_t *perf_find_hotspots(perf_buffer_t *buffer) {
    if (!buffer) return NULL;
    
    perf_hotspot_analysis_t *analysis = malloc(sizeof(perf_hotspot_analysis_t));
    if (!analysis) return NULL;
    
    memset(analysis, 0, sizeof(perf_hotspot_analysis_t));
    
    typedef struct {
        uint64_t function_addr;
        uint64_t total_duration;
        uint64_t call_count;
    } function_stat;
    
    function_stat func_stats[256];
    memset(func_stats, 0, sizeof(func_stats));
    size_t func_count = 0;
    
    for (size_t i = 0; i < buffer->sample_count; i++) {
        perf_sample_t *sample = &buffer->samples[i];
        
        if (sample->call_depth > 0) {
            uint64_t func_addr = sample->call_stack[0];
            
            int found_idx = -1;
            for (size_t j = 0; j < func_count; j++) {
                if (func_stats[j].function_addr == func_addr) {
                    found_idx = j;
                    break;
                }
            }
            
            if (found_idx >= 0) {
                func_stats[found_idx].total_duration += sample->duration_ns;
                func_stats[found_idx].call_count++;
            } else if (func_count < 256) {
                func_stats[func_count].function_addr = func_addr;
                func_stats[func_count].total_duration = sample->duration_ns;
                func_stats[func_count].call_count = 1;
                func_count++;
            }
        }
    }
    
    for (size_t i = 0; i < func_count && i < 100; i++) {
        analysis->hotspots[i].hotspot_function = func_stats[i].function_addr;
        analysis->hotspots[i].call_count = func_stats[i].call_count;
        analysis->hotspots[i].total_duration_ns = func_stats[i].total_duration;
        analysis->hotspots[i].self_duration_ns = func_stats[i].total_duration / func_stats[i].call_count;
    }
    
    analysis->hotspot_count = (func_count > 100) ? 100 : func_count;
    return analysis;
}

uint64_t perf_get_context_switch_overhead(perf_buffer_t *buffer) {
    if (!buffer || buffer->sample_count == 0) return 0;
    
    uint64_t total = 0;
    uint32_t count = 0;
    
    for (size_t i = 0; i < buffer->sample_count; i++) {
        if (buffer->samples[i].event_type == PERF_EVENT_CONTEXT_SWITCH) {
            total += buffer->samples[i].duration_ns;
            count++;
        }
    }
    
    return count > 0 ? total / count : 0;
}

uint64_t perf_get_syscall_latency(perf_buffer_t *buffer) {
    if (!buffer || buffer->sample_count == 0) return 0;
    
    uint64_t total = 0;
    uint32_t count = 0;
    
    for (size_t i = 0; i < buffer->sample_count; i++) {
        if (buffer->samples[i].event_type == PERF_EVENT_SYSCALL) {
            total += buffer->samples[i].duration_ns;
            count++;
        }
    }
    
    return count > 0 ? total / count : 0;
}

uint64_t perf_get_ipc_latency(perf_buffer_t *buffer) {
    if (!buffer || buffer->sample_count == 0) return 0;
    
    uint64_t total = 0;
    uint32_t count = 0;
    
    for (size_t i = 0; i < buffer->sample_count; i++) {
        if (buffer->samples[i].event_type == PERF_EVENT_IPC_MESSAGE) {
            total += buffer->samples[i].duration_ns;
            count++;
        }
    }
    
    return count > 0 ? total / count : 0;
}

bool perf_optimize_cpu_cache(void) {
    return true;
}

bool perf_optimize_memory_allocator(void) {
    return true;
}

bool perf_optimize_scheduler(void) {
    return true;
}

bool perf_optimize_ipc_bus(void) {
    return true;
}

void perf_report_statistics(perf_statistics_t *stats) {
    if (!stats) return;
    
    printf("Performance Statistics Report:\n");
    printf("  Total Context Switches: %lu\n", stats->total_context_switches);
    printf("  Total Page Faults: %lu\n", stats->total_page_faults);
    printf("  Total Cache Misses: %lu\n", stats->total_cache_misses);
    printf("  Peak Memory Usage: %lu bytes\n", stats->peak_memory_usage);
    printf("  Tracked Events: %zu\n", stats->event_count);
}

void perf_report_hotspots(perf_hotspot_analysis_t *hotspots) {
    if (!hotspots) return;
    
    printf("Hotspot Analysis Report:\n");
    printf("  Identified Hotspots: %zu\n", hotspots->hotspot_count);
    
    for (size_t i = 0; i < hotspots->hotspot_count && i < 10; i++) {
        printf("  #%zu: Function 0x%lx\n", i + 1, hotspots->hotspots[i].hotspot_function);
        printf("    Call Count: %lu\n", hotspots->hotspots[i].call_count);
        printf("    Total Duration: %lu ns\n", hotspots->hotspots[i].total_duration_ns);
    }
}

void perf_free_statistics(perf_statistics_t *stats) {
    free(stats);
}

void perf_free_hotspot_analysis(perf_hotspot_analysis_t *hotspots) {
    free(hotspots);
}

void perf_free_buffer(perf_buffer_t *buffer) {
    free(buffer);
}
