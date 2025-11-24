#include <security/secure_boot.h>

int sb_init(void) {
    return 0;
}

int sb_shutdown(void) {
    return 0;
}

int sb_enable_secure_boot(void) {
    return 0;
}

int sb_disable_secure_boot(void) {
    return 0;
}

int sb_is_secure_boot_enabled(void) {
    return 1;
}

int sb_verify_bootloader(void* bootloader, int size) {
    if (bootloader == NULL || size <= 0) return -1;
    return 1;
}

int sb_verify_kernel(void* kernel, int size) {
    if (kernel == NULL || size <= 0) return -1;
    return 1;
}

int sb_load_certificate(const char* cert_path) {
    if (cert_path == NULL) return -1;
    return 0;
}

int sb_verify_signature(void* data, int size, void* signature) {
    if (data == NULL || signature == NULL) return -1;
    return 1;
}

int sb_enable_tpm(void) {
    return 0;
}

int sb_disable_tpm(void) {
    return 0;
}

int sb_seal_to_tpm(void* data, int size) {
    if (data == NULL || size <= 0) return -1;
    return 0;
}

int sb_unseal_from_tpm(void* data, int size) {
    if (data == NULL || size <= 0) return -1;
    return 0;
}

int sb_extend_pcr(int pcr_index, void* data, int size) {
    if (data == NULL || size <= 0) return -1;
    return 0;
}

int sb_get_pcr(int pcr_index, void* pcr_value) {
    if (pcr_value == NULL) return -1;
    return 0;
}

int sb_measure_component(const char* name, void* data, int size) {
    if (name == NULL || data == NULL) return -1;
    return 0;
}
