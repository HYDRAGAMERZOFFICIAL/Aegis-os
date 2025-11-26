#[repr(u32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum CpuFeature {
    SSE = 1,
    SSE2 = 2,
    SSE3 = 3,
    SSSE3 = 4,
    SSE4_1 = 5,
    SSE4_2 = 6,
    AVX = 7,
    AVX2 = 8,
    AVX512 = 9,
    AES = 10,
    SHA = 11,
    RDRAND = 12,
    VMX = 13,
    SVM = 14,
    SMX = 15,
    PAE = 16,
    PSE36 = 17,
    HYPERVISOR = 18,
    TSC_DEADLINE = 19,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct Cpuid {
    pub eax: u32,
    pub ebx: u32,
    pub ecx: u32,
    pub edx: u32,
}
