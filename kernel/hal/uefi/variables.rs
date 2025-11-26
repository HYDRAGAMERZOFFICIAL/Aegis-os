pub struct UefiVariable {
    pub name: [u16; 256],
    pub vendor: [u8; 16],
    pub data: *const u8,
    pub data_size: u32,
}
