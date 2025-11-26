use core::ffi::c_void;

#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DeviceClass {
    Cpu = 0x01,
    Gpu = 0x02,
    Audio = 0x03,
    Block = 0x04,
    Usb = 0x05,
    Network = 0x06,
    Sensor = 0x07,
    Acpi = 0x08,
    Uefi = 0x09,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct Device {
    pub id: u32,
    pub class: DeviceClass,
    pub vendor_id: u16,
    pub device_id: u16,
    pub status: u8,
    pub priv_data: *const c_void,
}

impl Device {
    pub fn new(
        id: u32,
        class: DeviceClass,
        vendor_id: u16,
        device_id: u16,
    ) -> Self {
        Device {
            id,
            class,
            vendor_id,
            device_id,
            status: 0,
            priv_data: core::ptr::null(),
        }
    }

    pub fn is_active(&self) -> bool {
        self.status != 0
    }

    pub fn set_active(&mut self, active: bool) {
        self.status = if active { 1 } else { 0 };
    }
}
