#include <kernel/profiler.h>
#include <common/list.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct profile_sample {
    struct list_head list;
    const char *name;
    uint64_t start_time;
    uint64_t end_time;
    uint64_t duration;
    uint32_t sample_count;
    uint32_t max_duration;
    uint32_t min_duration;
} profile_sample_t;

static struct list_head profile_samples;
static int profiler_initialized = 0;

void profiler_init(void)
{
    if (profiler_initialized) return;
    
    INIT_LIST_HEAD(&profile_samples);
    profiler_initialized = 1;
}

uint64_t profiler_get_cpu_cycles(void)
{
    uint64_t cycles;
    asm volatile("rdtsc" : "=A" (cycles));
    return cycles;
}

void profiler_start(const char *name)
{
    if (!profiler_initialized) {
        profiler_init();
    }
    
    profile_sample_t *sample = (profile_sample_t *)malloc(sizeof(profile_sample_t));
    if (!sample) return;
    
    sample->name = name;
    sample->start_time = profiler_get_cpu_cycles();
    sample->end_time = 0;
    sample->duration = 0;
    sample->sample_count = 1;
    sample->max_duration = 0;
    sample->min_duration = UINT32_MAX;
    
    list_add_tail(&sample->list, &profile_samples);
}

void profiler_end(const char *name)
{
    if (!profiler_initialized) {
        return;
    }
    
    uint64_t end_time = profiler_get_cpu_cycles();
    struct list_head *pos;
    
    list_for_each(pos, &profile_samples) {
        profile_sample_t *sample = list_entry(pos, profile_sample_t, list);
        
        if (strcmp(sample->name, name) == 0 && sample->end_time == 0) {
            sample->end_time = end_time;
            sample->duration = end_time - sample->start_time;
            
            if (sample->duration > sample->max_duration) {
                sample->max_duration = sample->duration;
            }
            
            if (sample->duration < sample->min_duration) {
                sample->min_duration = sample->duration;
            }
            
            break;
        }
    }
}

void profiler_record_event(const char *name, uint32_t duration)
{
    if (!profiler_initialized) {
        profiler_init();
    }
    
    profile_sample_t *sample = (profile_sample_t *)malloc(sizeof(profile_sample_t));
    if (!sample) return;
    
    sample->name = name;
    sample->start_time = 0;
    sample->end_time = 0;
    sample->duration = duration;
    sample->sample_count = 1;
    sample->max_duration = duration;
    sample->min_duration = duration;
    
    list_add_tail(&sample->list, &profile_samples);
}

void profiler_print_report(void)
{
    if (!profiler_initialized || list_empty(&profile_samples)) {
        printf("Profiler not initialized or no samples recorded\n");
        return;
    }
    
    printf("\n=== Performance Profile Report ===\n");
    printf("%-30s | %-12s | %-12s | %-12s\n", "Operation", "Duration", "Max", "Min");
    printf("-------|-----------|-----------|-----------|------\n");
    
    struct list_head *pos;
    uint64_t total_time = 0;
    int sample_count = 0;
    
    list_for_each(pos, &profile_samples) {
        profile_sample_t *sample = list_entry(pos, profile_sample_t, list);
        
        printf("%-30s | %12llu | %12u | %12u\n",
               sample->name,
               sample->duration,
               sample->max_duration,
               sample->min_duration);
        
        total_time += sample->duration;
        sample_count++;
    }
    
    printf("\n");
    printf("Total Samples: %d\n", sample_count);
    printf("Total Time: %llu cycles\n", total_time);
    printf("Average Time: %llu cycles\n", sample_count > 0 ? total_time / sample_count : 0);
}

void profiler_clear(void)
{
    if (!profiler_initialized) {
        return;
    }
    
    struct list_head *pos, *tmp;
    
    list_for_each_safe(pos, tmp, &profile_samples) {
        profile_sample_t *sample = list_entry(pos, profile_sample_t, list);
        list_del(&sample->list);
        free(sample);
    }
}

uint64_t profiler_get_average_duration(const char *name)
{
    if (!profiler_initialized) {
        return 0;
    }
    
    struct list_head *pos;
    uint64_t total = 0;
    int count = 0;
    
    list_for_each(pos, &profile_samples) {
        profile_sample_t *sample = list_entry(pos, profile_sample_t, list);
        
        if (strcmp(sample->name, name) == 0) {
            total += sample->duration;
            count++;
        }
    }
    
    return count > 0 ? total / count : 0;
}

uint32_t profiler_get_max_duration(const char *name)
{
    if (!profiler_initialized) {
        return 0;
    }
    
    struct list_head *pos;
    uint32_t max = 0;
    
    list_for_each(pos, &profile_samples) {
        profile_sample_t *sample = list_entry(pos, profile_sample_t, list);
        
        if (strcmp(sample->name, name) == 0) {
            if (sample->max_duration > max) {
                max = sample->max_duration;
            }
        }
    }
    
    return max;
}

uint32_t profiler_get_min_duration(const char *name)
{
    if (!profiler_initialized) {
        return UINT32_MAX;
    }
    
    struct list_head *pos;
    uint32_t min = UINT32_MAX;
    
    list_for_each(pos, &profile_samples) {
        profile_sample_t *sample = list_entry(pos, profile_sample_t, list);
        
        if (strcmp(sample->name, name) == 0) {
            if (sample->min_duration < min) {
                min = sample->min_duration;
            }
        }
    }
    
    return min;
}

int profiler_sample_count(void)
{
    if (!profiler_initialized) {
        return 0;
    }
    
    int count = 0;
    struct list_head *pos;
    
    list_for_each(pos, &profile_samples) {
        count++;
    }
    
    return count;
}
