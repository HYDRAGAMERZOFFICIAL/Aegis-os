#ifndef AEGIS_KERNEL_AI_PREFETCH_H
#define AEGIS_KERNEL_AI_PREFETCH_H

#include <kernel/types.h>

typedef struct {
    char app_path[256];
    uint32_t launch_count;
    uint64_t avg_launch_time_us;
    float prediction_accuracy;
} prefetch_app_profile_t;

typedef struct {
    char app_path[256];
    uint32_t expected_pid;
    uint64_t prefetch_time_us;
} prefetch_prediction_t;

int prefetch_init(void);
int prefetch_enable(void);
int prefetch_disable(void);

int prefetch_learn_pattern(const char *app_path);
int prefetch_predict_next_app(prefetch_prediction_t *prediction);

int prefetch_preload_app(const char *app_path);
int prefetch_preload_library(const char *lib_path);

int prefetch_get_app_profile(const char *app_path, prefetch_app_profile_t *profile);
int prefetch_get_prediction_accuracy(float *accuracy);

int prefetch_optimize_by_time(uint32_t hour_of_day);
int prefetch_optimize_by_location(const char *location);

int prefetch_cache_shared_libs(void);
int prefetch_clear_cache(void);

uint64_t prefetch_get_preloaded_bytes(void);
int prefetch_set_max_preload_size(uint64_t size_bytes);

#endif
