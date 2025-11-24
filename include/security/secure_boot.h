#ifndef AEGIS_SECURE_BOOT_H
#define AEGIS_SECURE_BOOT_H

#include <kernel/types.h>

typedef enum {
    BOOT_MODE_SECURE,
    BOOT_MODE_DEBUG,
    BOOT_MODE_RECOVERY
} boot_mode_t;

typedef enum {
    VERIFY_OK,
    VERIFY_FAILED,
    VERIFY_REVOKED,
    VERIFY_EXPIRED
} verification_result_t;

typedef struct {
    u8 hash[32];
    u8 signature[256];
    u64 timestamp;
    verification_result_t result;
} code_signature_t;

typedef struct {
    u8 pcr[24];
    u64 timestamp;
    bool valid;
} tpm_pcr_t;

typedef struct {
    u8 public_key[256];
    u8 certificate[1024];
    u64 expiry;
    bool revoked;
} attestation_credential_t;

int secure_boot_init(void);
int secure_boot_verify_kernel(const u8 *kernel_image, u64 size);
int secure_boot_verify_driver(const char *driver_name, const u8 *driver_image, u64 size);
int secure_boot_verify_signature(const u8 *data, u64 data_size, const u8 *signature);
int secure_boot_enable_lockdown(void);
int secure_boot_load_tpm_pcrs(void);
int secure_boot_extend_pcr(u32 pcr_index, const u8 *data, u64 size);
tpm_pcr_t **secure_boot_get_pcr_values(u32 *count);
int secure_boot_attest_system(attestation_credential_t *cred);
int secure_boot_verify_attestation(attestation_credential_t *cred);
int secure_boot_enable_measured_boot(void);
int secure_boot_set_boot_mode(boot_mode_t mode);
boot_mode_t secure_boot_get_boot_mode(void);
int secure_boot_revoke_certificate(const u8 *cert_hash);

#endif
