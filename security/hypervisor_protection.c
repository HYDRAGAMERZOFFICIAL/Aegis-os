#include <security/hypervisor_protection.h>
#include <string.h>

int hv_init(void) { return 0; }
int hv_enable(void) { return 0; }
int hv_disable(void) { return 0; }
int hv_load_firmware(const char *firmware_path) { return 0; }
int hv_verify_firmware_signature(const char *firmware_path, uint8_t *is_valid) { return 0; }
int hv_protect_kernel_memory(uint64_t start_address, uint64_t size) { return 0; }
int hv_unprotect_kernel_memory(uint64_t start_address) { return 0; }
int hv_enable_memory_protection(void) { return 0; }
int hv_disable_memory_protection(void) { return 0; }
int hv_enable_code_injection_detection(void) { return 0; }
int hv_disable_code_injection_detection(void) { return 0; }
int hv_intercept_syscall(uint32_t syscall_num, uint8_t enabled) { return 0; }
int hv_get_interception_stats(char *stats, uint32_t max_len) { return 0; }
int hv_get_protection_info(hv_protection_info_t *info) { return 0; }
int hv_get_threat_log(hv_threat_log_t *threats, uint32_t *count, uint32_t max_threats) { return 0; }
int hv_handle_kernel_exception(uint64_t address, uint8_t *allow_continue) { return 0; }
int hv_isolate_malicious_kernel_code(uint64_t code_address) { return 0; }
int hv_attestation_verify(uint8_t *is_trusted) { return 0; }
int hv_get_security_report(char *report, uint32_t max_len) { return 0; }
