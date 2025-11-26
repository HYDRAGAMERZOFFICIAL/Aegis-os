#[repr(C)]
pub struct UsbDevice {
    pub vendor_id: u16,
    pub device_id: u16,
    pub bus: u8,
    pub port: u8,
}
