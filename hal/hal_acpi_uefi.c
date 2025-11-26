#include <string.h>
#include <stdlib.h>
#include "../include/hal/hal_acpi_uefi.h"

#define MAX_ACPI_DEVICES 128
#define MAX_ACPI_TABLES 64
#define MAX_UEFI_VARIABLES 256

typedef struct {
    uint32_t device_count;
    hal_acpi_device_t devices[MAX_ACPI_DEVICES];
    uint32_t table_count;
    hal_acpi_table_info_t tables[MAX_ACPI_TABLES];
    uint32_t variable_count;
    hal_uefi_variable_info_t variables[MAX_UEFI_VARIABLES];
    uint64_t rsdp_address;
    bool initialized;
    bool uefi_secure_boot;
    uint64_t device_counter;
} acpi_uefi_hal_state_t;

static acpi_uefi_hal_state_t acpi_uefi_state = {0};

hal_status_t hal_acpi_uefi_init(uint64_t rsdp_address) {
    if (acpi_uefi_state.initialized) {
        return HAL_OK;
    }
    
    memset(&acpi_uefi_state, 0, sizeof(acpi_uefi_hal_state_t));
    
    acpi_uefi_state.rsdp_address = rsdp_address;
    acpi_uefi_state.device_counter = 1000;
    acpi_uefi_state.uefi_secure_boot = true;
    
    acpi_uefi_state.device_count = 8;
    
    strcpy(acpi_uefi_state.devices[0].name, "PCI0");
    strcpy(acpi_uefi_state.devices[0].hid, "PNP0A08");
    acpi_uefi_state.devices[0].status = 0x0F;
    acpi_uefi_state.devices[0].present = true;
    acpi_uefi_state.devices[0].enabled = true;
    acpi_uefi_state.devices[0].power_state = 0;
    
    strcpy(acpi_uefi_state.devices[1].name, "EC0");
    strcpy(acpi_uefi_state.devices[1].hid, "PNP0C09");
    acpi_uefi_state.devices[1].status = 0x0F;
    acpi_uefi_state.devices[1].present = true;
    acpi_uefi_state.devices[1].enabled = true;
    
    strcpy(acpi_uefi_state.devices[2].name, "PWRB");
    strcpy(acpi_uefi_state.devices[2].hid, "PNP0C0C");
    acpi_uefi_state.devices[2].status = 0x0F;
    acpi_uefi_state.devices[2].irq = 0;
    acpi_uefi_state.devices[2].present = true;
    acpi_uefi_state.devices[2].enabled = true;
    
    strcpy(acpi_uefi_state.devices[3].name, "LNKA");
    strcpy(acpi_uefi_state.devices[3].hid, "PNP0C0F");
    acpi_uefi_state.devices[3].irq = 5;
    acpi_uefi_state.devices[3].present = true;
    acpi_uefi_state.devices[3].enabled = true;
    
    strcpy(acpi_uefi_state.devices[4].name, "LNKB");
    strcpy(acpi_uefi_state.devices[4].hid, "PNP0C0F");
    acpi_uefi_state.devices[4].irq = 10;
    acpi_uefi_state.devices[4].present = true;
    acpi_uefi_state.devices[4].enabled = true;
    
    strcpy(acpi_uefi_state.devices[5].name, "LNKC");
    strcpy(acpi_uefi_state.devices[5].hid, "PNP0C0F");
    acpi_uefi_state.devices[5].irq = 11;
    acpi_uefi_state.devices[5].present = true;
    acpi_uefi_state.devices[5].enabled = true;
    
    strcpy(acpi_uefi_state.devices[6].name, "LNKD");
    strcpy(acpi_uefi_state.devices[6].hid, "PNP0C0F");
    acpi_uefi_state.devices[6].irq = 9;
    acpi_uefi_state.devices[6].present = true;
    acpi_uefi_state.devices[6].enabled = true;
    
    strcpy(acpi_uefi_state.devices[7].name, "THERM");
    strcpy(acpi_uefi_state.devices[7].hid, "LNXTHERM");
    acpi_uefi_state.devices[7].status = 0x0F;
    acpi_uefi_state.devices[7].present = true;
    acpi_uefi_state.devices[7].enabled = true;
    
    acpi_uefi_state.table_count = 4;
    
    strcpy((char *)acpi_uefi_state.tables[0].signature, "DSDT");
    acpi_uefi_state.tables[0].length = 65536;
    acpi_uefi_state.tables[0].revision = 2;
    acpi_uefi_state.tables[0].valid = true;
    
    strcpy((char *)acpi_uefi_state.tables[1].signature, "FACP");
    acpi_uefi_state.tables[1].length = 244;
    acpi_uefi_state.tables[1].revision = 4;
    acpi_uefi_state.tables[1].valid = true;
    
    strcpy((char *)acpi_uefi_state.tables[2].signature, "MADT");
    acpi_uefi_state.tables[2].length = 2048;
    acpi_uefi_state.tables[2].revision = 3;
    acpi_uefi_state.tables[2].valid = true;
    
    strcpy((char *)acpi_uefi_state.tables[3].signature, "SSDT");
    acpi_uefi_state.tables[3].length = 32768;
    acpi_uefi_state.tables[3].revision = 2;
    acpi_uefi_state.tables[3].valid = true;
    
    acpi_uefi_state.variable_count = 3;
    
    strcpy((char *)acpi_uefi_state.variables[0].name, "BootCurrent");
    acpi_uefi_state.variables[0].attributes = 0x07;
    acpi_uefi_state.variables[0].value_size = 2;
    
    strcpy((char *)acpi_uefi_state.variables[1].name, "BootOrder");
    acpi_uefi_state.variables[1].attributes = 0x07;
    acpi_uefi_state.variables[1].value_size = 8;
    
    strcpy((char *)acpi_uefi_state.variables[2].name, "SecureBoot");
    acpi_uefi_state.variables[2].attributes = 0x07;
    acpi_uefi_state.variables[2].value_size = 1;
    
    acpi_uefi_state.initialized = true;
    
    return HAL_OK;
}

hal_status_t hal_acpi_uefi_fini(void) {
    if (!acpi_uefi_state.initialized) {
        return HAL_OK;
    }
    
    memset(&acpi_uefi_state, 0, sizeof(acpi_uefi_hal_state_t));
    
    return HAL_OK;
}

hal_status_t hal_acpi_enumerate_devices(hal_acpi_device_t *devices, uint32_t *device_count) {
    if (!devices || !device_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    uint32_t count = acpi_uefi_state.device_count;
    if (*device_count < count) {
        *device_count = count;
        return HAL_ERR_INVALID_ARG;
    }
    
    memcpy(devices, acpi_uefi_state.devices, count * sizeof(hal_acpi_device_t));
    *device_count = count;
    
    return HAL_OK;
}

hal_status_t hal_acpi_get_device(const char *path, hal_acpi_device_t *device) {
    if (!path || !device) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < acpi_uefi_state.device_count; i++) {
        if (strcmp(acpi_uefi_state.devices[i].name, path) == 0) {
            memcpy(device, &acpi_uefi_state.devices[i], sizeof(hal_acpi_device_t));
            return HAL_OK;
        }
    }
    
    return HAL_ERR_DEVICE_FAILED;
}

hal_status_t hal_acpi_evaluate_method(const char *path, const hal_acpi_arg_t *args, 
                                      uint32_t arg_count, hal_acpi_value_t *result) {
    if (!path || !result) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    result->type = HAL_ACPI_ARG_INTEGER;
    result->value.integer = 0;
    
    return HAL_OK;
}

hal_status_t hal_acpi_get_table(const char *signature, uint32_t instance, 
                                 hal_acpi_table_info_t *table_info) {
    if (!signature || !table_info) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    uint32_t found_count = 0;
    for (uint32_t i = 0; i < acpi_uefi_state.table_count; i++) {
        if (strncmp((const char *)acpi_uefi_state.tables[i].signature, signature, 4) == 0) {
            if (found_count == instance) {
                memcpy(table_info, &acpi_uefi_state.tables[i], sizeof(hal_acpi_table_info_t));
                return HAL_OK;
            }
            found_count++;
        }
    }
    
    return HAL_ERR_DEVICE_FAILED;
}

hal_status_t hal_acpi_notify_device(const char *path, uint32_t notify_value) {
    if (!path) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < acpi_uefi_state.device_count; i++) {
        if (strcmp(acpi_uefi_state.devices[i].name, path) == 0) {
            return HAL_OK;
        }
    }
    
    return HAL_ERR_DEVICE_FAILED;
}

hal_status_t hal_acpi_enable_device(const char *path) {
    if (!path) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < acpi_uefi_state.device_count; i++) {
        if (strcmp(acpi_uefi_state.devices[i].name, path) == 0) {
            acpi_uefi_state.devices[i].enabled = true;
            return HAL_OK;
        }
    }
    
    return HAL_ERR_DEVICE_FAILED;
}

hal_status_t hal_acpi_disable_device(const char *path) {
    if (!path) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < acpi_uefi_state.device_count; i++) {
        if (strcmp(acpi_uefi_state.devices[i].name, path) == 0) {
            acpi_uefi_state.devices[i].enabled = false;
            return HAL_OK;
        }
    }
    
    return HAL_ERR_DEVICE_FAILED;
}

hal_status_t hal_acpi_get_power_state(const char *path, uint8_t *state) {
    if (!path || !state) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < acpi_uefi_state.device_count; i++) {
        if (strcmp(acpi_uefi_state.devices[i].name, path) == 0) {
            *state = acpi_uefi_state.devices[i].power_state;
            return HAL_OK;
        }
    }
    
    return HAL_ERR_DEVICE_FAILED;
}

hal_status_t hal_acpi_set_power_state(const char *path, uint8_t state) {
    if (!path) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < acpi_uefi_state.device_count; i++) {
        if (strcmp(acpi_uefi_state.devices[i].name, path) == 0) {
            acpi_uefi_state.devices[i].power_state = state;
            return HAL_OK;
        }
    }
    
    return HAL_ERR_DEVICE_FAILED;
}

hal_status_t hal_uefi_get_variable(const char *name, const hal_guid_t *guid, 
                                    uint8_t *data, uint32_t *size) {
    if (!name || !guid || !data || !size) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < acpi_uefi_state.variable_count; i++) {
        if (strcmp(acpi_uefi_state.variables[i].name, name) == 0) {
            if (*size < acpi_uefi_state.variables[i].value_size) {
                *size = acpi_uefi_state.variables[i].value_size;
                return HAL_ERR_INVALID_ARG;
            }
            *size = acpi_uefi_state.variables[i].value_size;
            memset(data, 0, *size);
            return HAL_OK;
        }
    }
    
    return HAL_ERR_DEVICE_FAILED;
}

hal_status_t hal_uefi_set_variable(const char *name, const hal_guid_t *guid, 
                                    const uint8_t *data, uint32_t size) {
    if (!name || !guid || !data) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < acpi_uefi_state.variable_count; i++) {
        if (strcmp(acpi_uefi_state.variables[i].name, name) == 0) {
            acpi_uefi_state.variables[i].value_size = size;
            return HAL_OK;
        }
    }
    
    if (acpi_uefi_state.variable_count < MAX_UEFI_VARIABLES) {
        strcpy((char *)acpi_uefi_state.variables[acpi_uefi_state.variable_count].name, name);
        acpi_uefi_state.variables[acpi_uefi_state.variable_count].value_size = size;
        acpi_uefi_state.variable_count++;
        return HAL_OK;
    }
    
    return HAL_ERR_NO_MEMORY;
}

hal_status_t hal_uefi_delete_variable(const char *name, const hal_guid_t *guid) {
    if (!name || !guid) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    for (uint32_t i = 0; i < acpi_uefi_state.variable_count; i++) {
        if (strcmp(acpi_uefi_state.variables[i].name, name) == 0) {
            if (i < acpi_uefi_state.variable_count - 1) {
                memmove(&acpi_uefi_state.variables[i], 
                        &acpi_uefi_state.variables[i + 1],
                        (acpi_uefi_state.variable_count - i - 1) * sizeof(hal_uefi_variable_info_t));
            }
            acpi_uefi_state.variable_count--;
            return HAL_OK;
        }
    }
    
    return HAL_ERR_DEVICE_FAILED;
}

hal_status_t hal_uefi_enumerate_variables(hal_uefi_variable_info_t *variables, 
                                          uint32_t *variable_count) {
    if (!variables || !variable_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    uint32_t count = acpi_uefi_state.variable_count;
    if (*variable_count < count) {
        *variable_count = count;
        return HAL_ERR_INVALID_ARG;
    }
    
    memcpy(variables, acpi_uefi_state.variables, count * sizeof(hal_uefi_variable_info_t));
    *variable_count = count;
    
    return HAL_OK;
}

hal_status_t hal_uefi_get_memory_map(hal_uefi_memory_map_t *map) {
    if (!map) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    map->descriptor_count = 0;
    map->descriptors = NULL;
    
    return HAL_OK;
}

hal_status_t hal_uefi_get_time(hal_uefi_time_t *time) {
    if (!time) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    time->year = 2024;
    time->month = 11;
    time->day = 26;
    time->hour = 19;
    time->minute = 35;
    time->second = 46;
    
    return HAL_OK;
}

hal_status_t hal_uefi_set_time(const hal_uefi_time_t *time) {
    if (!time) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    return HAL_OK;
}

hal_status_t hal_uefi_get_boot_options(uint16_t *boot_order, uint32_t *boot_count) {
    if (!boot_order || !boot_count) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    boot_order[0] = 0x0001;
    boot_order[1] = 0x0002;
    boot_order[2] = 0x0003;
    *boot_count = 3;
    
    return HAL_OK;
}

hal_status_t hal_uefi_set_boot_options(const uint16_t *boot_order, uint32_t boot_count) {
    if (!boot_order || boot_count == 0) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    return HAL_OK;
}

hal_status_t hal_uefi_is_secure_boot_enabled(bool *enabled) {
    if (!enabled) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    *enabled = acpi_uefi_state.uefi_secure_boot;
    
    return HAL_OK;
}

hal_status_t hal_uefi_verify_signature(const uint8_t *data, uint32_t data_size, 
                                       const uint8_t *signature, uint32_t sig_size, 
                                       bool *valid) {
    if (!data || !signature || !valid) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    *valid = (sig_size > 0) ? true : false;
    
    return HAL_OK;
}

hal_status_t hal_uefi_load_image(const char *path, uint8_t **image, uint32_t *size) {
    if (!path || !image || !size) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    *image = NULL;
    *size = 0;
    
    return HAL_OK;
}

hal_status_t hal_tpm_extend_pcr(uint8_t pcr_index, const uint8_t *data, uint32_t data_size) {
    if (!data || pcr_index > 23) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    return HAL_OK;
}

hal_status_t hal_tpm_read_pcr(uint8_t pcr_index, uint8_t *pcr_value, uint32_t *size) {
    if (!pcr_value || !size || pcr_index > 23) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    if (*size < 32) {
        *size = 32;
        return HAL_ERR_INVALID_ARG;
    }
    
    memset(pcr_value, 0, 32);
    *size = 32;
    
    return HAL_OK;
}

hal_status_t hal_tpm_get_quote(uint8_t *pcr_indices, uint32_t pcr_count, 
                               uint8_t *quote, uint32_t *quote_size) {
    if (!pcr_indices || !quote || !quote_size) {
        return HAL_ERR_INVALID_ARG;
    }
    
    if (!acpi_uefi_state.initialized) {
        return HAL_ERR_DEVICE_FAILED;
    }
    
    if (*quote_size < 256) {
        *quote_size = 256;
        return HAL_ERR_INVALID_ARG;
    }
    
    memset(quote, 0, 256);
    *quote_size = 256;
    
    return HAL_OK;
}
