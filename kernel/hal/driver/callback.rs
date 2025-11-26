use super::descriptor::{DriverCallbackType, DriverDescriptor};
use crate::error::{HalError, Result};
use core::ffi::c_void;

pub struct DriverCallback {
    descriptor: DriverDescriptor,
}

impl DriverCallback {
    pub fn new(descriptor: DriverDescriptor) -> Result<Self> {
        descriptor.validate()?;
        Ok(DriverCallback { descriptor })
    }

    pub fn call_init(&self, device: *const c_void, data: *const c_void) -> Result<()> {
        if let Some(callback) = self.descriptor.init {
            let status = callback(device, DriverCallbackType::Init, data);
            if status != 0 {
                return Err(HalError::from(status));
            }
        }
        Ok(())
    }

    pub fn call_probe(&self, device: *const c_void, data: *const c_void) -> Result<()> {
        if let Some(callback) = self.descriptor.probe {
            let status = callback(device, DriverCallbackType::Probe, data);
            if status != 0 {
                return Err(HalError::from(status));
            }
        }
        Ok(())
    }

    pub fn call_suspend(&self, device: *const c_void, data: *const c_void) -> Result<()> {
        if let Some(callback) = self.descriptor.suspend {
            let status = callback(device, DriverCallbackType::Suspend, data);
            if status != 0 {
                return Err(HalError::from(status));
            }
        }
        Ok(())
    }

    pub fn call_resume(&self, device: *const c_void, data: *const c_void) -> Result<()> {
        if let Some(callback) = self.descriptor.resume {
            let status = callback(device, DriverCallbackType::Resume, data);
            if status != 0 {
                return Err(HalError::from(status));
            }
        }
        Ok(())
    }

    pub fn call_remove(&self, device: *const c_void, data: *const c_void) -> Result<()> {
        if let Some(callback) = self.descriptor.remove {
            let status = callback(device, DriverCallbackType::Remove, data);
            if status != 0 {
                return Err(HalError::from(status));
            }
        }
        Ok(())
    }
}
