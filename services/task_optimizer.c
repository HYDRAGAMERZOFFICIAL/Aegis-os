#include <services/task_optimizer.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static optimize_mode_t g_current_mode = OPTIMIZE_MODE_WORK;
static uint32_t g_cpu_boost_percent = 0;
static uint32_t g_gpu_boost_percent = 0;
static uint8_t g_updates_enabled = 1;
static uint8_t g_background_services_paused = 0;

int taskopt_init(void) {
    g_current_mode = OPTIMIZE_MODE_WORK;
    g_cpu_boost_percent = 0;
    g_gpu_boost_percent = 0;
    g_updates_enabled = 1;
    g_background_services_paused = 0;
    return 0;
}

int taskopt_detect_active_game(uint32_t *pid) {
    if (!pid) return -1;
    *pid = (g_current_mode == OPTIMIZE_MODE_GAME) ? 1042 : 0;
    return 0;
}

int taskopt_detect_workload(optimize_mode_t *mode) {
    if (!mode) return -1;
    *mode = g_current_mode;
    return 0;
}

int taskopt_apply_optimization(optimize_mode_t mode) {
    switch (mode) {
        case OPTIMIZE_MODE_GAME: return taskopt_optimize_for_game(1042);
        case OPTIMIZE_MODE_WORK: return taskopt_optimize_for_work();
        case OPTIMIZE_MODE_STREAMING: return taskopt_optimize_for_streaming();
        case OPTIMIZE_MODE_VIDEO_EDIT: return taskopt_optimize_for_video_edit();
        case OPTIMIZE_MODE_LOW_POWER: return taskopt_optimize_for_low_power();
        default: return -1;
    }
}

int taskopt_optimize_for_game(uint32_t game_pid) {
    (void)game_pid;
    g_current_mode = OPTIMIZE_MODE_GAME;
    g_cpu_boost_percent = 25;
    g_gpu_boost_percent = 30;
    g_background_services_paused = 1;
    g_updates_enabled = 0;
    return 0;
}

int taskopt_optimize_for_work(void) {
    g_current_mode = OPTIMIZE_MODE_WORK;
    g_cpu_boost_percent = 0;
    g_gpu_boost_percent = 0;
    g_background_services_paused = 0;
    g_updates_enabled = 1;
    return 0;
}

int taskopt_optimize_for_streaming(void) {
    g_current_mode = OPTIMIZE_MODE_STREAMING;
    g_cpu_boost_percent = 15;
    g_gpu_boost_percent = 15;
    g_background_services_paused = 0;
    return 0;
}

int taskopt_optimize_for_video_edit(void) {
    g_current_mode = OPTIMIZE_MODE_VIDEO_EDIT;
    g_cpu_boost_percent = 20;
    g_gpu_boost_percent = 25;
    return 0;
}

int taskopt_optimize_for_low_power(void) {
    g_current_mode = OPTIMIZE_MODE_LOW_POWER;
    g_cpu_boost_percent = 0;
    g_gpu_boost_percent = 0;
    g_updates_enabled = 0;
    return 0;
}

int taskopt_close_background_apps(app_optimization_t *closed_apps, uint32_t *count, uint32_t max_apps) {
    if (!closed_apps || !count) return -1;
    uint32_t n = (max_apps > 2) ? 2 : max_apps;
    for (uint32_t i = 0; i < n; i++) {
        closed_apps[i].pid = 200 + i;
        closed_apps[i].closed = 1;
        closed_apps[i].paused = 0;
        closed_apps[i].priority_adjusted = 1;
        closed_apps[i].cpu_boosted = 0;
        closed_apps[i].gpu_boosted = 0;
    }
    *count = n;
    return 0;
}

int taskopt_pause_background_services(void) {
    g_background_services_paused = 1;
    return 0;
}

int taskopt_resume_background_services(void) {
    g_background_services_paused = 0;
    return 0;
}

int taskopt_disable_updates(void) {
    g_updates_enabled = 0;
    return 0;
}

int taskopt_enable_updates(void) {
    g_updates_enabled = 1;
    return 0;
}

int taskopt_boost_cpu(uint32_t percent) {
    if (percent > 100) return -1;
    g_cpu_boost_percent = percent;
    return 0;
}

int taskopt_boost_gpu(uint32_t percent) {
    if (percent > 100) return -1;
    g_gpu_boost_percent = percent;
    return 0;
}

int taskopt_get_optimization_status(optimize_mode_t *current_mode, char *status, uint32_t max_len) {
    if (current_mode) *current_mode = g_current_mode;
    if (status && max_len > 0) {
        snprintf(status, max_len, "Mode: %d | CPU Boost: +%u%% | GPU Boost: +%u%% | Background Paused: %d",
                 (int)g_current_mode, g_cpu_boost_percent, g_gpu_boost_percent, g_background_services_paused);
    }
    return 0;
}

int taskopt_restore_default(void) {
    return taskopt_optimize_for_work();
}
