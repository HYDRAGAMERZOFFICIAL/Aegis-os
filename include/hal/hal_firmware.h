#ifndef AEGIS_HAL_FIRMWARE_H
#define AEGIS_HAL_FIRMWARE_H

#include <stdint.h>
#include <stdbool.h>
#include "hal.h"

typedef enum {
    HAL_FIRMWARE_UNKNOWN = 0,
    HAL_FIRMWARE_BIOS,
    HAL_FIRMWARE_UEFI_32,
    HAL_FIRMWARE_UEFI_64,
} hal_firmware_type_t;

typedef struct {
    hal_firmware_type_t type;
    uint32_t version;
    uint8_t *vendor;
    uint8_t *release_date;
    bool secure_boot_enabled;
    bool tpm_enabled;
    uint8_t tpm_version;
} hal_firmware_info_t;

typedef struct {
    uint8_t *signature;
    uint32_t signature_size;
    uint8_t *certificate;
    uint32_t cert_size;
    bool verified;
} hal_firmware_signature_t;

typedef struct {
    uint64_t base_address;
    uint32_t size;
    uint8_t *vendor;
    uint8_t *product;
    uint8_t *serial;
} hal_smbios_entry_t;

typedef struct {
    uint8_t *table_name;
    uint32_t table_size;
    uint32_t table_version;
    bool valid;
} hal_acpi_table_t;

typedef struct {
    uint64_t base;
    uint32_t size;
    uint8_t type;
    uint8_t attr;
} hal_memory_map_entry_t;

typedef struct {
    uint32_t entry_count;
    hal_memory_map_entry_t *entries;
} hal_memory_map_t;

hal_status_t hal_firmware_init(void);
hal_status_t hal_firmware_get_info(hal_firmware_info_t *info);

hal_status_t hal_firmware_is_uefi(bool *is_uefi);
hal_status_t hal_firmware_is_secure_boot_enabled(bool *enabled);
hal_status_t hal_firmware_enable_secure_boot(void);
hal_status_t hal_firmware_disable_secure_boot(void);

hal_status_t hal_firmware_validate_signature(const uint8_t *data, uint32_t size, 
                                             hal_firmware_signature_t *sig);
hal_status_t hal_firmware_load_microcode(const uint8_t *microcode, uint32_t size);
hal_status_t hal_firmware_update(const uint8_t *image, uint32_t size);

hal_status_t hal_acpi_init(void);
hal_status_t hal_acpi_fini(void);
hal_status_t hal_acpi_get_table(const uint8_t *signature, uint32_t instance, 
                                 hal_acpi_table_t *table);
hal_status_t hal_acpi_eval_object(const uint8_t *path, uint64_t *result);
hal_status_t hal_acpi_get_device_info(const uint8_t *device_path, uint8_t *info_buf, 
                                       uint32_t buf_size);
hal_status_t hal_acpi_notify_device(const uint8_t *device_path, uint8_t notify_value);

hal_status_t hal_smbios_init(void);
hal_status_t hal_smbios_get_entry(uint8_t type, uint8_t index, 
                                   hal_smbios_entry_t *entry);

hal_status_t hal_uefi_get_runtime_services(void);
hal_status_t hal_uefi_get_boot_services(void);
hal_status_t hal_uefi_get_variable(const uint8_t *name, const uint8_t *guid, 
                                    uint8_t *data, uint32_t *size);
hal_status_t hal_uefi_set_variable(const uint8_t *name, const uint8_t *guid, 
                                    const uint8_t *data, uint32_t size);
hal_status_t hal_uefi_get_memory_map(hal_memory_map_t *map);
hal_status_t hal_uefi_exit_boot_services(void);

hal_status_t hal_firmware_get_tpm_info(uint8_t *version, uint8_t *status);
hal_status_t hal_firmware_extend_pcr(uint8_t pcr_index, const uint8_t *data, 
                                      uint32_t data_size);
hal_status_t hal_firmware_read_pcr(uint8_t pcr_index, uint8_t *pcr_value, 
                                    uint32_t *pcr_size);

#endif
