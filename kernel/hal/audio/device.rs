#[repr(C)]
pub struct AudioDevice {
    pub id: u32,
    pub name: [u8; 256],
    pub device_type: u32,
}
