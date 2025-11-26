#ifndef AEGIS_HAL_USB_H
#define AEGIS_HAL_USB_H

#include <stdint.h>
#include <stdbool.h>
#include "hal.h"

typedef enum {
    HAL_USB_SPEED_UNKNOWN = 0,
    HAL_USB_SPEED_LOW = 1,
    HAL_USB_SPEED_FULL = 2,
    HAL_USB_SPEED_HIGH = 3,
    HAL_USB_SPEED_SUPER = 4,
    HAL_USB_SPEED_SUPER_PLUS = 5,
} hal_usb_speed_t;

typedef enum {
    HAL_USB_TRANSFER_CONTROL = 0,
    HAL_USB_TRANSFER_ISOCHRONOUS = 1,
    HAL_USB_TRANSFER_BULK = 2,
    HAL_USB_TRANSFER_INTERRUPT = 3,
} hal_usb_transfer_type_t;

typedef enum {
    HAL_USB_EVENT_CONNECTED = 1,
    HAL_USB_EVENT_DISCONNECTED = 2,
    HAL_USB_EVENT_TRANSFER_COMPLETE = 3,
    HAL_USB_EVENT_ERROR = 4,
} hal_usb_event_type_t;

typedef struct {
    uint8_t bus;
    uint8_t device;
    uint8_t port;
    uint16_t vendor_id;
    uint16_t product_id;
    uint8_t device_class;
    uint8_t device_subclass;
    uint8_t device_protocol;
    hal_usb_speed_t speed;
    uint8_t *manufacturer;
    uint8_t *product;
    uint8_t *serial_number;
    uint8_t config_count;
    bool is_connected;
    uint8_t address;
} hal_usb_device_t;

typedef struct {
    uint8_t endpoint_address;
    hal_usb_transfer_type_t type;
    uint16_t max_packet_size;
    uint8_t interval;
    bool is_active;
} hal_usb_endpoint_t;

typedef struct {
    uint8_t config_value;
    uint8_t interface_count;
    uint8_t *description;
} hal_usb_config_t;

typedef struct {
    uint8_t interface_number;
    uint8_t alternate_setting;
    uint8_t endpoint_count;
    hal_usb_endpoint_t *endpoints;
} hal_usb_interface_t;

typedef struct {
    uint64_t transfer_id;
    uint8_t endpoint;
    uint8_t direction;
    uint32_t data_size;
    bool is_complete;
    hal_status_t status;
    uint32_t bytes_transferred;
} hal_usb_transfer_t;

typedef struct {
    uint8_t request_type;
    uint8_t request;
    uint16_t value;
    uint16_t index;
    uint16_t length;
    const void *data;
} hal_usb_control_request_t;

typedef void (*hal_usb_callback_t)(hal_usb_event_type_t event, uint8_t bus, uint8_t device, 
                                    void *context);
typedef void (*hal_usb_transfer_callback_t)(uint64_t transfer_id, hal_status_t status, 
                                             uint32_t bytes_transferred, void *context);

hal_status_t hal_usb_init(void);
hal_status_t hal_usb_fini(void);
hal_status_t hal_usb_enumerate_devices(uint32_t *device_count);
hal_status_t hal_usb_get_device_info(uint8_t bus, uint8_t device, hal_usb_device_t *info);

hal_status_t hal_usb_get_config(uint8_t bus, uint8_t device, uint8_t config_index, 
                                 hal_usb_config_t *config);
hal_status_t hal_usb_set_config(uint8_t bus, uint8_t device, uint8_t config_value);
hal_status_t hal_usb_get_interface(uint8_t bus, uint8_t device, uint8_t interface, 
                                    hal_usb_interface_t *iface);
hal_status_t hal_usb_set_interface(uint8_t bus, uint8_t device, uint8_t interface, 
                                    uint8_t alternate_setting);

hal_status_t hal_usb_control_transfer(uint8_t bus, uint8_t device, 
                                       const hal_usb_control_request_t *request, 
                                       void *data, uint32_t timeout_ms);
hal_status_t hal_usb_bulk_transfer(uint8_t bus, uint8_t device, uint8_t endpoint, 
                                    void *data, uint32_t data_len, uint32_t *transferred, 
                                    uint32_t timeout_ms);
hal_status_t hal_usb_interrupt_transfer(uint8_t bus, uint8_t device, uint8_t endpoint, 
                                         void *data, uint32_t data_len, uint32_t *transferred, 
                                         uint32_t timeout_ms);
hal_status_t hal_usb_isochronous_transfer(uint8_t bus, uint8_t device, uint8_t endpoint, 
                                           void *data, uint32_t data_len, 
                                           uint32_t *transferred, uint32_t timeout_ms);

hal_status_t hal_usb_bulk_transfer_async(uint8_t bus, uint8_t device, uint8_t endpoint, 
                                          void *data, uint32_t data_len, 
                                          hal_usb_transfer_callback_t callback, void *context, 
                                          uint64_t *transfer_id);
hal_status_t hal_usb_interrupt_transfer_async(uint8_t bus, uint8_t device, uint8_t endpoint, 
                                               void *data, uint32_t data_len, 
                                               hal_usb_transfer_callback_t callback, 
                                               void *context, uint64_t *transfer_id);

hal_status_t hal_usb_cancel_transfer(uint64_t transfer_id);
hal_status_t hal_usb_get_transfer_status(uint64_t transfer_id, hal_usb_transfer_t *transfer);

hal_status_t hal_usb_register_callback(hal_usb_callback_t callback, void *context);
hal_status_t hal_usb_unregister_callback(void);

hal_status_t hal_usb_reset_device(uint8_t bus, uint8_t device);
hal_status_t hal_usb_clear_stall(uint8_t bus, uint8_t device, uint8_t endpoint);

hal_status_t hal_usb_get_string_descriptor(uint8_t bus, uint8_t device, uint8_t string_id, 
                                            uint16_t lang_id, uint8_t *buffer, 
                                            uint32_t buffer_size);

hal_status_t hal_hid_get_report(uint8_t bus, uint8_t device, uint8_t report_id, 
                                 void *report, uint32_t report_size);
hal_status_t hal_hid_set_report(uint8_t bus, uint8_t device, uint8_t report_id, 
                                 const void *report, uint32_t report_size);
hal_status_t hal_hid_get_idle(uint8_t bus, uint8_t device, uint8_t *idle);
hal_status_t hal_hid_set_idle(uint8_t bus, uint8_t device, uint8_t idle);
hal_status_t hal_hid_get_protocol(uint8_t bus, uint8_t device, uint8_t *protocol);
hal_status_t hal_hid_set_protocol(uint8_t bus, uint8_t device, uint8_t protocol);

hal_status_t hal_usb_port_get_status(uint8_t bus, uint8_t port, uint16_t *status);
hal_status_t hal_usb_port_set_feature(uint8_t bus, uint8_t port, uint16_t feature);
hal_status_t hal_usb_port_clear_feature(uint8_t bus, uint8_t port, uint16_t feature);

#endif
