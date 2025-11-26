#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub enum AcpiTableType {
    DSDT,
    FACP,
    MADT,
    SSDT,
}

#[repr(C)]
pub struct AcpiTable {
    pub table_type: AcpiTableType,
    pub data: *const u8,
    pub length: u32,
}
