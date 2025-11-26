#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct CpuTopology {
    pub package_count: u8,
    pub cores_per_package: u8,
    pub threads_per_core: u8,
    pub logical_cpus: u16,
    pub l1_cache_size: u16,
    pub l2_cache_size: u16,
    pub l3_cache_size: u32,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct CpuInfo {
    pub vendor: *const u8,
    pub brand: *const u8,
    pub family: u8,
    pub model: u8,
    pub stepping: u8,
    pub feature_bitmap: u64,
}

impl Default for CpuTopology {
    fn default() -> Self {
        CpuTopology {
            package_count: 1,
            cores_per_package: 8,
            threads_per_core: 2,
            logical_cpus: 16,
            l1_cache_size: 32,
            l2_cache_size: 256,
            l3_cache_size: 16384,
        }
    }
}
