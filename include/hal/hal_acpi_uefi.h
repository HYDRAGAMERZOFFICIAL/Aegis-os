#ifndef AEGIS_HAL_ACPI_UEFI_H
#define AEGIS_HAL_ACPI_UEFI_H

#include <stdint.h>
#include <stdbool.h>
#include "hal.h"

#define HAL_ACPI_MAX_DEVICES 128
#define HAL_ACPI_MAX_TABLES 64
#define HAL_GUID_SIZE 16

typedef struct {
    uint8_t guid[HAL_GUID_SIZE];
} hal_guid_t;

typedef enum {
    HAL_ACPI_ARG_NULL = 0,
    HAL_ACPI_ARG_INTEGER,
    HAL_ACPI_ARG_STRING,
    HAL_ACPI_ARG_BUFFER,
    HAL_ACPI_ARG_PACKAGE,
} hal_acpi_arg_type_t;

typedef struct {
    hal_acpi_arg_type_t type;
    union {
        uint64_t integer;
        const char *string;
        struct {
            uint8_t *buffer;
            uint32_t size;
        } buffer;
        void *package;
    } value;
} hal_acpi_arg_t;

typedef struct {
    hal_acpi_arg_type_t type;
    union {
        uint64_t integer;
        char *string;
        struct {
            uint8_t *buffer;
            uint32_t size;
        } buffer;
        void *package;
    } value;
} hal_acpi_value_t;

typedef struct {
    char name[64];
    char hid[16];
    char uid[16];
    uint8_t status;
    uint64_t address;
    uint32_t irq;
    bool present;
    bool enabled;
    bool removable;
    uint8_t power_state;
} hal_acpi_device_t;

typedef struct {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint32_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
    uint8_t *table_data;
    bool valid;
} hal_acpi_table_info_t;

typedef struct {
    uint8_t guid[HAL_GUID_SIZE];
    char name[256];
    uint8_t attributes;
    uint64_t value_size;
} hal_uefi_variable_info_t;

typedef struct {
    uint64_t base_address;
    uint64_t size;
    uint32_t type;
    uint32_t attributes;
} hal_uefi_memory_descriptor_t;

typedef struct {
    uint32_t descriptor_size;
    uint32_t descriptor_count;
    hal_uefi_memory_descriptor_t *descriptors;
} hal_uefi_memory_map_t;

typedef struct {
    uint32_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} hal_uefi_time_t;

hal_status_t hal_acpi_uefi_init(uint64_t rsdp_address);
hal_status_t hal_acpi_uefi_fini(void);

hal_status_t hal_acpi_enumerate_devices(hal_acpi_device_t *devices, uint32_t *device_count);
hal_status_t hal_acpi_get_device(const char *path, hal_acpi_device_t *device);
hal_status_t hal_acpi_evaluate_method(const char *path, const hal_acpi_arg_t *args, 
                                      uint32_t arg_count, hal_acpi_value_t *result);
hal_status_t hal_acpi_get_table(const char *signature, uint32_t instance, 
                                 hal_acpi_table_info_t *table_info);
hal_status_t hal_acpi_notify_device(const char *path, uint32_t notify_value);
hal_status_t hal_acpi_enable_device(const char *path);
hal_status_t hal_acpi_disable_device(const char *path);
hal_status_t hal_acpi_get_power_state(const char *path, uint8_t *state);
hal_status_t hal_acpi_set_power_state(const char *path, uint8_t state);

hal_status_t hal_uefi_get_variable(const char *name, const hal_guid_t *guid, 
                                    uint8_t *data, uint32_t *size);
hal_status_t hal_uefi_set_variable(const char *name, const hal_guid_t *guid, 
                                    const uint8_t *data, uint32_t size);
hal_status_t hal_uefi_delete_variable(const char *name, const hal_guid_t *guid);
hal_status_t hal_uefi_enumerate_variables(hal_uefi_variable_info_t *variables, 
                                          uint32_t *variable_count);

hal_status_t hal_uefi_get_memory_map(hal_uefi_memory_map_t *map);
hal_status_t hal_uefi_get_time(hal_uefi_time_t *time);
hal_status_t hal_uefi_set_time(const hal_uefi_time_t *time);
hal_status_t hal_uefi_get_boot_options(uint16_t *boot_order, uint32_t *boot_count);
hal_status_t hal_uefi_set_boot_options(const uint16_t *boot_order, uint32_t boot_count);

hal_status_t hal_uefi_is_secure_boot_enabled(bool *enabled);
hal_status_t hal_uefi_verify_signature(const uint8_t *data, uint32_t data_size, 
                                       const uint8_t *signature, uint32_t sig_size, 
                                       bool *valid);
hal_status_t hal_uefi_load_image(const char *path, uint8_t **image, uint32_t *size);

hal_status_t hal_tpm_extend_pcr(uint8_t pcr_index, const uint8_t *data, uint32_t data_size);
hal_status_t hal_tpm_read_pcr(uint8_t pcr_index, uint8_t *pcr_value, uint32_t *size);
hal_status_t hal_tpm_get_quote(uint8_t *pcr_indices, uint32_t pcr_count, 
                               uint8_t *quote, uint32_t *quote_size);

#endif
