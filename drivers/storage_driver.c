#include <drivers/storage_driver.h>
#include <string.h>

int storage_driver_init(void) { return 0; }
int storage_driver_shutdown(void) { return 0; }

int storage_enumerate_devices(storage_device_info_t *devices, uint32_t *count, uint32_t max_devices) { return 0; }
storage_device_t *storage_open_device(uint32_t device_id) { return 0; }
int storage_close_device(storage_device_t *dev) { return 0; }

int storage_read_sectors(storage_device_t *dev, uint64_t sector, uint32_t count, void *buffer) { return 0; }
int storage_write_sectors(storage_device_t *dev, uint64_t sector, uint32_t count, const void *buffer) { return 0; }

int storage_read_sectors_async(storage_device_t *dev, storage_io_request_t *req, 
                               storage_completion_callback_fn callback, void *ctx) { return 0; }
int storage_write_sectors_async(storage_device_t *dev, storage_io_request_t *req,
                                storage_completion_callback_fn callback, void *ctx) { return 0; }

int storage_get_device_info(storage_device_t *dev, storage_device_info_t *info) { return 0; }
storage_status_t storage_get_status(storage_device_t *dev) { return STORAGE_STATUS_OK; }

int storage_format_device(storage_device_t *dev, const char *filesystem_type) { return 0; }
int storage_check_filesystem(storage_device_t *dev, uint8_t repair) { return 0; }

int storage_enable_encryption(storage_device_t *dev, const uint8_t *key, uint32_t key_len) { return 0; }
int storage_disable_encryption(storage_device_t *dev) { return 0; }
int storage_is_encrypted(storage_device_t *dev) { return 0; }

int storage_enable_auto_encrypt_external(void) { return 0; }
int storage_disable_auto_encrypt_external(void) { return 0; }
int storage_get_auto_encrypt_status(void) { return 0; }

int storage_trim_device(storage_device_t *dev) { return 0; }
int storage_flush_cache(storage_device_t *dev) { return 0; }

int storage_set_power_mode(storage_device_t *dev, uint8_t power_state) { return 0; }
int storage_get_power_mode(storage_device_t *dev) { return 0; }

uint32_t storage_get_queue_depth(storage_device_t *dev) { return 0; }
int storage_cancel_io(storage_device_t *dev, uint64_t io_id) { return 0; }

int storage_smart_get_health(storage_device_t *dev, char *health_data, uint32_t max_len) { return 0; }
int storage_smart_enable_monitoring(storage_device_t *dev) { return 0; }
