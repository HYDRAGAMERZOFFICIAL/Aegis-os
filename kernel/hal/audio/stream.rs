#[repr(C)]
pub struct AudioStream {
    pub id: u32,
    pub sample_rate: u32,
    pub channels: u8,
    pub bit_depth: u8,
}
