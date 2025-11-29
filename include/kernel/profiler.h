#ifndef AEGIS_KERNEL_PROFILER_H
#define AEGIS_KERNEL_PROFILER_H

#include <stdint.h>
#include <limits.h>

void profiler_init(void);

uint64_t profiler_get_cpu_cycles(void);

void profiler_start(const char *name);

void profiler_end(const char *name);

void profiler_record_event(const char *name, uint32_t duration);

void profiler_print_report(void);

void profiler_clear(void);

uint64_t profiler_get_average_duration(const char *name);

uint32_t profiler_get_max_duration(const char *name);

uint32_t profiler_get_min_duration(const char *name);

int profiler_sample_count(void);

#endif
