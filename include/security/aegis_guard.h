#ifndef AEGIS_SECURITY_AEGISGUARD_H
#define AEGIS_SECURITY_AEGISGUARD_H

#include <kernel/types.h>

typedef enum {
    GUARD_THREAT_RANSOMWARE = 1,
    GUARD_THREAT_ESCALATION = 2,
    GUARD_THREAT_EXPLOIT = 3,
    GUARD_THREAT_KEYLOGGER = 4,
    GUARD_THREAT_UNKNOWN = 5,
} guard_threat_type_t;

typedef struct {
    uint32_t threat_id;
    guard_threat_type_t threat_type;
    float confidence;
    const char *description;
    uint64_t detected_time;
    uint8_t auto_blocked;
} guard_threat_t;

int guard_init(void);
int guard_shutdown(void);
int guard_enable(void);
int guard_disable(void);
int guard_is_enabled(void);

int guard_scan_process(uint32_t pid, guard_threat_t *threats, uint32_t *count, uint32_t max_threats);
int guard_scan_file(const char *path, guard_threat_t *threats, uint32_t *count, uint32_t max_threats);
int guard_scan_network(guard_threat_t *threats, uint32_t *count, uint32_t max_threats);

int guard_block_behavior(uint32_t pid, const char *behavior_pattern);
int guard_quarantine_process(uint32_t pid);
int guard_kill_threat(guard_threat_t *threat);

int guard_get_detected_threats(guard_threat_t *threats, uint32_t *count, uint32_t max_threats);
int guard_clear_threat_history(void);

int guard_train_ml_model(const char *training_data_path);
int guard_get_ml_accuracy(float *accuracy);

int guard_set_sensitivity(uint8_t level);
int guard_get_sensitivity(uint8_t *level);

int guard_export_threat_log(const char *output_file);

#endif
