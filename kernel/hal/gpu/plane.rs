#[repr(u32)]
pub enum PlaneType {
    Primary = 0,
    Overlay = 1,
    Cursor = 2,
}

#[repr(C)]
pub struct Plane {
    pub id: u32,
    pub plane_type: PlaneType,
    pub enabled: bool,
}
