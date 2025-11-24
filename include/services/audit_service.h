#ifndef AEGIS_AUDIT_SERVICE_H
#define AEGIS_AUDIT_SERVICE_H

#include <kernel/types.h>

typedef enum {
    AUDIT_EVENT_LOGIN,
    AUDIT_EVENT_LOGOUT,
    AUDIT_EVENT_FILE_ACCESS,
    AUDIT_EVENT_FILE_MODIFY,
    AUDIT_EVENT_PERMISSION_CHANGE,
    AUDIT_EVENT_NETWORK_ACCESS,
    AUDIT_EVENT_PROCESS_EXEC,
    AUDIT_EVENT_SECURITY_ALERT
} audit_event_type_t;

typedef enum {
    COMPLIANCE_MODE_NONE,
    COMPLIANCE_MODE_HIPAA,
    COMPLIANCE_MODE_GDPR,
    COMPLIANCE_MODE_ISO27001,
    COMPLIANCE_MODE_SOC2
} compliance_mode_t;

typedef struct {
    u64 event_id;
    u64 timestamp;
    audit_event_type_t event_type;
    u64 user_id;
    u64 source_pid;
    const char *resource;
    const char *action;
    bool success;
    u8 hash[32];
} audit_event_t;

int audit_init(void);
int audit_log_event(audit_event_type_t type, u64 user_id, u64 pid, const char *resource, const char *action, bool success);
audit_event_t **audit_get_events(u64 start_time, u64 end_time, u32 *count);
int audit_search_events(audit_event_type_t type, u64 user_id, u32 *count);
int audit_export_report(const char *filename, u64 start_time, u64 end_time);
int audit_set_compliance_mode(compliance_mode_t mode);
int audit_verify_event_integrity(u64 event_id);
int audit_enable_tamper_protection(void);
int audit_generate_compliance_report(compliance_mode_t mode, const char *output_file);
int audit_clear_old_events(u64 older_than_timestamp);
u64 audit_get_event_count(void);

#endif
