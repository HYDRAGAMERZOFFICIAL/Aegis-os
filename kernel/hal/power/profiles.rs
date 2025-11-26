#[repr(u32)]
#[derive(Debug, Clone, Copy)]
pub enum PowerProfile {
    BatterySaver = 0,
    Balanced = 1,
    Performance = 2,
}
