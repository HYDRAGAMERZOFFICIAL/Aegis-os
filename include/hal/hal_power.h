#ifndef AEGIS_HAL_POWER_H
#define AEGIS_HAL_POWER_H

#include <stdint.h>
#include <stdbool.h>
#include "hal.h"

typedef enum {
    HAL_POWER_STATE_S0 = 0,
    HAL_POWER_STATE_S1 = 1,
    HAL_POWER_STATE_S2 = 2,
    HAL_POWER_STATE_S3 = 3,
    HAL_POWER_STATE_S4 = 4,
    HAL_POWER_STATE_S5 = 5,
    HAL_POWER_STATE_G3 = 6,
} hal_power_state_t;

typedef enum {
    HAL_CPU_IDLE_C0 = 0,
    HAL_CPU_IDLE_C1 = 1,
    HAL_CPU_IDLE_C1E = 2,
    HAL_CPU_IDLE_C2 = 3,
    HAL_CPU_IDLE_C3 = 4,
    HAL_CPU_IDLE_C6 = 5,
    HAL_CPU_IDLE_C7 = 6,
    HAL_CPU_IDLE_C8 = 7,
    HAL_CPU_IDLE_C9 = 8,
    HAL_CPU_IDLE_C10 = 9,
} hal_cpu_idle_state_t;

typedef enum {
    HAL_TURBO_STATE_ENABLED = 1,
    HAL_TURBO_STATE_DISABLED = 0,
} hal_turbo_state_t;

typedef struct {
    uint32_t voltage_mv;
    uint32_t frequency_mhz;
    uint8_t power_level;
} hal_power_pstate_t;

typedef struct {
    uint8_t pstate_count;
    hal_power_pstate_t *pstates;
    uint8_t current_pstate;
} hal_power_pstate_table_t;

typedef struct {
    bool battery_present;
    uint8_t battery_percent;
    uint16_t voltage_mv;
    int32_t current_ma;
    int32_t temperature_celsius;
    uint64_t capacity_mwh;
    uint64_t remaining_mwh;
    bool is_charging;
    uint32_t charge_time_remaining_min;
    uint32_t discharge_time_remaining_min;
} hal_battery_info_t;

typedef struct {
    uint32_t power_input_w;
    uint32_t power_cpu_w;
    uint32_t power_gpu_w;
    uint32_t power_memory_w;
    uint32_t power_other_w;
    uint32_t total_power_w;
    uint32_t estimated_runtime_min;
} hal_power_metrics_t;

typedef struct {
    uint8_t thermal_zone_id;
    int32_t current_temperature_celsius;
    int32_t trip_point_celsius;
    int32_t critical_temperature_celsius;
    bool is_cooling;
    uint8_t fan_speed_percent;
} hal_thermal_zone_t;

typedef struct {
    uint32_t zone_count;
    hal_thermal_zone_t *zones;
} hal_thermal_info_t;

typedef struct {
    uint8_t fan_id;
    uint32_t current_rpm;
    uint32_t min_rpm;
    uint32_t max_rpm;
    uint8_t current_percent;
    bool is_controllable;
} hal_fan_info_t;

typedef enum {
    HAL_POWER_PROFILE_BATTERY_SAVER = 0,
    HAL_POWER_PROFILE_BALANCED = 1,
    HAL_POWER_PROFILE_PERFORMANCE = 2,
} hal_power_profile_t;

typedef struct {
    bool in_transition;
    hal_power_state_t current_state;
    hal_power_state_t target_state;
    uint32_t devices_quiesced;
    uint32_t total_devices;
    bool hibernation_image_encrypted;
} hal_suspend_resume_state_t;

typedef struct {
    uint32_t wake_sources;
    bool rtc_enabled;
    bool usb_enabled;
    bool power_button_enabled;
    bool keyboard_enabled;
    bool mouse_enabled;
    bool network_enabled;
    bool disk_enabled;
} hal_wakeup_sources_t;

typedef void (*hal_power_event_callback_t)(uint32_t event_type, void *context);
typedef void (*hal_battery_callback_t)(const hal_battery_info_t *battery_info, void *context);
typedef void (*hal_thermal_callback_t)(const hal_thermal_zone_t *zone, void *context);
typedef void (*hal_device_quiesce_callback_t)(uint32_t device_id, void *context);
typedef void (*hal_resume_callback_t)(void *context);

hal_status_t hal_power_init(void);
hal_status_t hal_power_fini(void);

hal_status_t hal_power_get_system_state(hal_power_state_t *state);
hal_status_t hal_power_set_system_state(hal_power_state_t state);
hal_status_t hal_power_sleep(uint32_t sleep_type);
hal_status_t hal_power_hibernate(void);
hal_status_t hal_power_shutdown(void);
hal_status_t hal_power_reboot(void);

hal_status_t hal_power_get_pstate_table(uint8_t cpu_id, hal_power_pstate_table_t *table);
hal_status_t hal_power_set_pstate(uint8_t cpu_id, uint8_t pstate);
hal_status_t hal_power_get_current_pstate(uint8_t cpu_id, uint8_t *pstate);

hal_status_t hal_power_set_cpu_idle_state(uint8_t cpu_id, hal_cpu_idle_state_t state);
hal_status_t hal_power_get_cpu_idle_state(uint8_t cpu_id, hal_cpu_idle_state_t *state);

hal_status_t hal_power_enable_turbo(uint8_t cpu_id);
hal_status_t hal_power_disable_turbo(uint8_t cpu_id);
hal_status_t hal_power_get_turbo_state(uint8_t cpu_id, hal_turbo_state_t *state);

hal_status_t hal_battery_get_info(hal_battery_info_t *info);
hal_status_t hal_battery_set_charge_threshold(uint8_t start_percent, uint8_t stop_percent);
hal_status_t hal_battery_get_charge_threshold(uint8_t *start_percent, uint8_t *stop_percent);
hal_status_t hal_battery_set_charging_enabled(bool enabled);
hal_status_t hal_battery_is_charging_enabled(bool *enabled);

hal_status_t hal_power_get_metrics(hal_power_metrics_t *metrics);
hal_status_t hal_power_start_power_profiling(void);
hal_status_t hal_power_stop_power_profiling(void);
hal_status_t hal_power_get_power_profile_data(void *buffer, uint32_t buffer_size, 
                                               uint32_t *bytes_written);

hal_status_t hal_thermal_get_info(hal_thermal_info_t *info);
hal_status_t hal_thermal_get_zone_info(uint8_t zone_id, hal_thermal_zone_t *zone);
hal_status_t hal_thermal_set_trip_point(uint8_t zone_id, int32_t temperature_celsius);

hal_status_t hal_fan_get_count(uint8_t *count);
hal_status_t hal_fan_get_info(uint8_t fan_id, hal_fan_info_t *info);
hal_status_t hal_fan_set_speed(uint8_t fan_id, uint8_t speed_percent);
hal_status_t hal_fan_set_auto_mode(uint8_t fan_id);
hal_status_t hal_fan_set_manual_mode(uint8_t fan_id);

hal_status_t hal_power_register_callback(uint32_t event_types, 
                                          hal_power_event_callback_t callback, void *context);
hal_status_t hal_power_unregister_callback(hal_power_event_callback_t callback);
hal_status_t hal_battery_register_callback(hal_battery_callback_t callback, void *context);
hal_status_t hal_thermal_register_callback(hal_thermal_callback_t callback, void *context);

hal_status_t hal_power_get_estimated_battery_life(uint32_t *minutes);
hal_status_t hal_power_enable_power_saving(void);
hal_status_t hal_power_disable_power_saving(void);
hal_status_t hal_power_is_power_saving_enabled(bool *enabled);

hal_status_t hal_power_set_profile(hal_power_profile_t profile);
hal_status_t hal_power_get_profile(hal_power_profile_t *profile);

hal_status_t hal_power_prepare_suspend(hal_power_state_t target_state);
hal_status_t hal_power_quiesce_device(uint32_t device_id);
hal_status_t hal_power_quiesce_all_devices(void);
hal_status_t hal_power_register_quiesce_callback(uint32_t device_id, 
                                                 hal_device_quiesce_callback_t callback, 
                                                 void *context);
hal_status_t hal_power_complete_suspend(hal_power_state_t target_state);
hal_status_t hal_power_resume_device(uint32_t device_id);
hal_status_t hal_power_resume_all_devices(void);
hal_status_t hal_power_register_resume_callback(hal_resume_callback_t callback, void *context);
hal_status_t hal_power_verify_resume_integrity(bool *valid);

hal_status_t hal_power_get_suspend_state(hal_suspend_resume_state_t *state);
hal_status_t hal_power_set_wakeup_sources(const hal_wakeup_sources_t *sources);
hal_status_t hal_power_get_wakeup_sources(hal_wakeup_sources_t *sources);

hal_status_t hal_power_throttle_cpu(uint8_t cpu_id, uint8_t percent);
hal_status_t hal_power_throttle_gpu(uint8_t gpu_id, uint8_t percent);
hal_status_t hal_power_unthrottle_device(uint32_t device_id);

#endif
