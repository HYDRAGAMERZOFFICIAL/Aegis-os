#[repr(C)]
pub struct HidDescriptor {
    pub class_code: u8,
    pub subclass_code: u8,
    pub protocol_code: u8,
}
