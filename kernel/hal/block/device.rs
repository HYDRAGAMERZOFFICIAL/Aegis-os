#[repr(C)]
pub struct BlockDevice {
    pub id: u32,
    pub name: [u8; 256],
    pub size_sectors: u64,
    pub sector_size: u32,
}
