#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct CpuStats {
    pub cpu_id: u8,
    pub idle_cycles: u64,
    pub total_cycles: u64,
    pub temperature_celsius: u32,
    pub power_state: u8,
}

impl Default for CpuStats {
    fn default() -> Self {
        CpuStats {
            cpu_id: 0,
            idle_cycles: 0,
            total_cycles: 0,
            temperature_celsius: 0,
            power_state: 0,
        }
    }
}
