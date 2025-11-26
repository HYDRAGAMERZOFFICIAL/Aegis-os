#[repr(C)]
pub struct Partition {
    pub id: u32,
    pub start_sector: u64,
    pub size_sectors: u64,
    pub partition_type: u32,
}
