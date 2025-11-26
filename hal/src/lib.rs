#![no_std]
#![allow(non_camel_case_types)]

pub mod ffi;
pub mod error;
pub mod cpu;
pub mod acpi;
pub mod uefi;
pub mod gpu;
pub mod audio;
pub mod storage;
pub mod usb;
pub mod power;
pub mod firmware;

pub use error::{HalStatus, HalError};
pub use ffi::*;

extern "C" {
    pub fn memset(s: *mut u8, c: i32, n: usize) -> *mut u8;
    pub fn memcpy(dest: *mut u8, src: *const u8, n: usize) -> *mut u8;
    pub fn memmove(dest: *mut u8, src: *const u8, n: usize) -> *mut u8;
}

pub struct HalCore {
    initialized: bool,
}

impl HalCore {
    pub const fn new() -> Self {
        HalCore {
            initialized: false,
        }
    }
}
