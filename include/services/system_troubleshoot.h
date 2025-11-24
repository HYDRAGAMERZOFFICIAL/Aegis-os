#ifndef AEGIS_SERVICES_TROUBLESHOOT_H
#define AEGIS_SERVICES_TROUBLESHOOT_H

#include <kernel/types.h>

typedef enum {
    DIAG_TYPE_MALWARE = 1,
    DIAG_TYPE_MEMORY = 2,
    DIAG_TYPE_NETWORK = 3,
    DIAG_TYPE_DRIVERS = 4,
    DIAG_TYPE_UPDATES = 5,
    DIAG_TYPE_DISK = 6,
    DIAG_TYPE_PERFORMANCE = 7,
} diag_type_t;

typedef struct {
    diag_type_t type;
    uint8_t status;
    char message[256];
    uint8_t auto_fixable;
} diagnosis_result_t;

int troubleshoot_init(void);
int troubleshoot_run_full_scan(diagnosis_result_t *results, uint32_t *count, uint32_t max_results);

int troubleshoot_scan_malware(diagnosis_result_t *result);
int troubleshoot_scan_memory(diagnosis_result_t *result);
int troubleshoot_scan_network(diagnosis_result_t *result);
int troubleshoot_scan_drivers(diagnosis_result_t *result);
int troubleshoot_check_updates(diagnosis_result_t *result);
int troubleshoot_check_disk(diagnosis_result_t *result);
int troubleshoot_analyze_performance(diagnosis_result_t *result);

int troubleshoot_auto_fix(diag_type_t issue_type);
int troubleshoot_get_fix_options(diag_type_t issue_type, char *options, uint32_t max_len);

int troubleshoot_enable_background_monitoring(void);
int troubleshoot_disable_background_monitoring(void);

int troubleshoot_get_last_scan_results(diagnosis_result_t *results, uint32_t *count, uint32_t max_results);
int troubleshoot_export_report(const char *output_file);

#endif
