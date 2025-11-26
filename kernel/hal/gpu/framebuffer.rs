#[repr(C)]
pub struct Framebuffer {
    pub id: u32,
    pub width: u32,
    pub height: u32,
    pub pitch: u32,
    pub pixel_format: u32,
    pub gpu_vaddr: u64,
}
