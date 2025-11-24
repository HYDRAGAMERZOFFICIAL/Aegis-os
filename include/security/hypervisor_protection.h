#ifndef AEGIS_SECURITY_HYPERVISOR_H
#define AEGIS_SECURITY_HYPERVISOR_H

#include <kernel/types.h>

typedef enum {
    HV_LEVEL_FIRMWARE = 0,
    HV_LEVEL_HYPERVISOR = 1,
    HV_LEVEL_KERNEL = 2,
} hv_protection_level_t;

typedef enum {
    HV_THREAT_KERNEL_EXPLOIT = 1,
    HV_THREAT_KERNEL_ROOTKIT = 2,
    HV_THREAT_MEMORY_TAMPERING = 3,
    HV_THREAT_CODE_INJECTION = 4,
} hv_threat_type_t;

typedef struct {
    hv_protection_level_t level;
    uint8_t enabled;
    uint64_t protected_regions;
    uint32_t interception_count;
} hv_protection_info_t;

typedef struct {
    hv_threat_type_t threat_type;
    uint32_t caller_pid;
    uint64_t target_address;
    uint8_t blocked;
    uint64_t detected_time;
} hv_threat_log_t;

int hv_init(void);
int hv_enable(void);
int hv_disable(void);

int hv_load_firmware(const char *firmware_path);
int hv_verify_firmware_signature(const char *firmware_path, uint8_t *is_valid);

int hv_protect_kernel_memory(uint64_t start_address, uint64_t size);
int hv_unprotect_kernel_memory(uint64_t start_address);

int hv_enable_memory_protection(void);
int hv_disable_memory_protection(void);

int hv_enable_code_injection_detection(void);
int hv_disable_code_injection_detection(void);

int hv_intercept_syscall(uint32_t syscall_num, uint8_t enabled);
int hv_get_interception_stats(char *stats, uint32_t max_len);

int hv_get_protection_info(hv_protection_info_t *info);
int hv_get_threat_log(hv_threat_log_t *threats, uint32_t *count, uint32_t max_threats);

int hv_handle_kernel_exception(uint64_t address, uint8_t *allow_continue);
int hv_isolate_malicious_kernel_code(uint64_t code_address);

int hv_attestation_verify(uint8_t *is_trusted);
int hv_get_security_report(char *report, uint32_t max_len);

#endif
