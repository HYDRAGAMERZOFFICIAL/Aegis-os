#[repr(u32)]
#[derive(Debug, Clone, Copy)]
pub enum PowerState {
    S0 = 0,
    S1 = 1,
    S3 = 3,
    S4 = 4,
    S5 = 5,
}
