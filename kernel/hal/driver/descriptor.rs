use crate::error::{HalError, Result};
use core::ffi::c_void;

#[repr(u32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DriverCallbackType {
    Init = 0,
    Probe = 1,
    Suspend = 2,
    Resume = 3,
    Remove = 4,
    Shutdown = 5,
}

pub type DriverCallbackFn = extern "C" fn(
    device: *const c_void,
    callback_type: DriverCallbackType,
    callback_data: *const c_void,
) -> i32;

#[repr(C)]
pub struct DriverDescriptor {
    pub driver_id: u32,
    pub driver_name: *const u8,
    pub version: u32,
    pub flags: u32,
    
    pub init: Option<DriverCallbackFn>,
    pub probe: Option<DriverCallbackFn>,
    pub suspend: Option<DriverCallbackFn>,
    pub resume: Option<DriverCallbackFn>,
    pub remove: Option<DriverCallbackFn>,
    
    pub priority: u32,
}

impl DriverDescriptor {
    pub fn validate(&self) -> Result<()> {
        if self.driver_id == 0 {
            return Err(HalError::InvalidArg);
        }

        if self.driver_name.is_null() {
            return Err(HalError::InvalidArg);
        }

        if self.probe.is_none() {
            return Err(HalError::InvalidArg);
        }

        Ok(())
    }

    pub fn name(&self) -> &'static str {
        unsafe {
            let mut len = 0;
            let mut ptr = self.driver_name;
            while *ptr != 0 && len < 256 {
                len += 1;
                ptr = ptr.add(1);
            }
            core::str::from_utf8_unchecked(core::slice::from_raw_parts(
                self.driver_name as *const u8,
                len,
            ))
        }
    }
}
