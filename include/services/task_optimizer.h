#ifndef AEGIS_SERVICES_TASK_OPTIMIZER_H
#define AEGIS_SERVICES_TASK_OPTIMIZER_H

#include <kernel/types.h>

typedef enum {
    OPTIMIZE_MODE_GAME = 1,
    OPTIMIZE_MODE_WORK = 2,
    OPTIMIZE_MODE_STREAMING = 3,
    OPTIMIZE_MODE_VIDEO_EDIT = 4,
    OPTIMIZE_MODE_LOW_POWER = 5,
} optimize_mode_t;

typedef struct {
    uint32_t pid;
    uint8_t closed;
    uint8_t paused;
    uint8_t priority_adjusted;
    uint8_t cpu_boosted;
    uint8_t gpu_boosted;
} app_optimization_t;

int taskopt_init(void);
int taskopt_detect_active_game(uint32_t *pid);
int taskopt_detect_workload(optimize_mode_t *mode);

int taskopt_apply_optimization(optimize_mode_t mode);
int taskopt_optimize_for_game(uint32_t game_pid);
int taskopt_optimize_for_work(void);
int taskopt_optimize_for_streaming(void);
int taskopt_optimize_for_video_edit(void);
int taskopt_optimize_for_low_power(void);

int taskopt_close_background_apps(app_optimization_t *closed_apps, uint32_t *count, uint32_t max_apps);
int taskopt_pause_background_services(void);
int taskopt_resume_background_services(void);

int taskopt_disable_updates(void);
int taskopt_enable_updates(void);

int taskopt_boost_cpu(uint32_t percent);
int taskopt_boost_gpu(uint32_t percent);

int taskopt_get_optimization_status(optimize_mode_t *current_mode, char *status, uint32_t max_len);
int taskopt_restore_default(void);

#endif
