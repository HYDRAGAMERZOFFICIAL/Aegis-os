#[repr(C)]
pub struct BatteryInfo {
    pub capacity_mah: u32,
    pub current_mah: u32,
    pub voltage_mv: u32,
    pub health: u8,
    pub status: u8,
}
