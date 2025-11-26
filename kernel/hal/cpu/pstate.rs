#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct PowerState {
    pub cpu_id: u8,
    pub frequency_mhz: u32,
    pub current_pstate: u8,
    pub max_pstate: u8,
    pub turbo_enabled: bool,
}

impl Default for PowerState {
    fn default() -> Self {
        PowerState {
            cpu_id: 0,
            frequency_mhz: 3400,
            current_pstate: 0,
            max_pstate: 15,
            turbo_enabled: false,
        }
    }
}
