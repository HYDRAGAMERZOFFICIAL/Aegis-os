#![allow(dead_code)]

use kernel_hal::driver::{DriverDescriptor, DriverCallbackType, Device, DeviceClass};
use kernel_hal::capability::{CapabilitySystem, Permission};
use kernel_hal::error::{HalError, Result};
use core::ffi::c_void;

const NVME_DRIVER_ID: u32 = 0x1001;

pub struct NvmeDevice {
    pci_vendor: u16,
    pci_device: u16,
    bar0_addr: u64,
    queue_depth: u16,
}

pub struct NvmeDriver {
    devices: [Option<NvmeDevice>; 16],
    device_count: usize,
    cap_system: CapabilitySystem,
}

impl NvmeDriver {
    pub fn new() -> Self {
        NvmeDriver {
            devices: [const { None }; 16],
            device_count: 0,
            cap_system: CapabilitySystem::new(),
        }
    }

    pub fn register_device(&mut self, device: NvmeDevice) -> Result<()> {
        if self.device_count >= 16 {
            return Err(HalError::NoMemory);
        }

        self.devices[self.device_count] = Some(device);
        self.device_count += 1;
        Ok(())
    }

    pub fn enumerate_devices(&self) -> usize {
        self.device_count
    }

    pub fn submit_read_request(
        &self,
        device_index: usize,
        lba: u64,
        length: u32,
        buffer: *mut u8,
        token: &kernel_hal::capability::CapabilityToken,
    ) -> Result<()> {
        if device_index >= self.device_count {
            return Err(HalError::InvalidArg);
        }

        self.cap_system.verify_token(token)?;

        if !self.cap_system.has_permission(token, Permission::READ) {
            return Err(HalError::PermissionDenied);
        }

        if buffer.is_null() {
            return Err(HalError::InvalidArg);
        }

        Ok(())
    }

    pub fn submit_write_request(
        &self,
        device_index: usize,
        lba: u64,
        length: u32,
        buffer: *const u8,
        token: &kernel_hal::capability::CapabilityToken,
    ) -> Result<()> {
        if device_index >= self.device_count {
            return Err(HalError::InvalidArg);
        }

        self.cap_system.verify_token(token)?;

        if !self.cap_system.has_permission(token, Permission::WRITE) {
            return Err(HalError::PermissionDenied);
        }

        if buffer.is_null() {
            return Err(HalError::InvalidArg);
        }

        Ok(())
    }
}

static mut NVME_DRIVER_INSTANCE: Option<NvmeDriver> = None;

extern "C" fn nvme_driver_init(
    _device: *const c_void,
    _callback_type: DriverCallbackType,
    _callback_data: *const c_void,
) -> i32 {
    unsafe {
        NVME_DRIVER_INSTANCE = Some(NvmeDriver::new());
    }
    0
}

extern "C" fn nvme_driver_probe(
    device_ptr: *const c_void,
    _callback_type: DriverCallbackType,
    _callback_data: *const c_void,
) -> i32 {
    if device_ptr.is_null() {
        return HalError::InvalidArg.into();
    }

    unsafe {
        if let Some(driver) = NVME_DRIVER_INSTANCE.as_mut() {
            let nvme_dev = NvmeDevice {
                pci_vendor: 0x8086,
                pci_device: 0x0953,
                bar0_addr: 0xFED00000,
                queue_depth: 32,
            };

            if let Err(e) = driver.register_device(nvme_dev) {
                return e.into();
            }
        }
    }
    0
}

extern "C" fn nvme_driver_suspend(
    _device: *const c_void,
    _callback_type: DriverCallbackType,
    _callback_data: *const c_void,
) -> i32 {
    0
}

extern "C" fn nvme_driver_resume(
    _device: *const c_void,
    _callback_type: DriverCallbackType,
    _callback_data: *const c_void,
) -> i32 {
    0
}

extern "C" fn nvme_driver_remove(
    _device: *const c_void,
    _callback_type: DriverCallbackType,
    _callback_data: *const c_void,
) -> i32 {
    unsafe {
        NVME_DRIVER_INSTANCE = None;
    }
    0
}

pub fn nvme_driver_descriptor() -> DriverDescriptor {
    DriverDescriptor {
        driver_id: NVME_DRIVER_ID,
        driver_name: b"nvme\0".as_ptr(),
        version: 0x010000,
        flags: 0x00,
        init: Some(nvme_driver_init),
        probe: Some(nvme_driver_probe),
        suspend: Some(nvme_driver_suspend),
        resume: Some(nvme_driver_resume),
        remove: Some(nvme_driver_remove),
        priority: 200,
    }
}

pub fn register_nvme_driver() -> Result<()> {
    let descriptor = nvme_driver_descriptor();
    descriptor.validate()?;
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_nvme_driver_creation() {
        let driver = NvmeDriver::new();
        assert_eq!(driver.enumerate_devices(), 0);
    }

    #[test]
    fn test_nvme_device_registration() {
        let mut driver = NvmeDriver::new();
        let device = NvmeDevice {
            pci_vendor: 0x8086,
            pci_device: 0x0953,
            bar0_addr: 0xFED00000,
            queue_depth: 32,
        };

        assert!(driver.register_device(device).is_ok());
        assert_eq!(driver.enumerate_devices(), 1);
    }

    #[test]
    fn test_nvme_descriptor_valid() {
        let descriptor = nvme_driver_descriptor();
        assert!(descriptor.validate().is_ok());
        assert_eq!(descriptor.driver_id, NVME_DRIVER_ID);
        assert_eq!(descriptor.priority, 200);
    }
}
