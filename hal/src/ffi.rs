use crate::error::{HalError, HalStatus};

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct hal_cpu_topology_t {
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
pub struct hal_cpuid_t {
    pub eax: u32,
    pub ebx: u32,
    pub ecx: u32,
    pub edx: u32,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct hal_cpu_pstate_t {
    pub cpu_id: u8,
    pub frequency_mhz: u32,
    pub current_pstate: u8,
    pub max_pstate: u8,
    pub turbo_enabled: bool,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct hal_cpu_stats_t {
    pub cpu_id: u8,
    pub idle_cycles: u64,
    pub total_cycles: u64,
    pub temperature_celsius: u32,
    pub power_state: u8,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct hal_cpu_info_t {
    pub vendor: *const u8,
    pub brand: *const u8,
    pub family: u8,
    pub model: u8,
    pub stepping: u8,
    pub feature_bitmap: u64,
}

pub type hal_cpu_mask_t = u64;

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub enum hal_cpu_feature_t {
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

#[no_mangle]
pub extern "C" fn hal_cpu_init() -> HalStatus {
    crate::cpu::CpuSubsystem::init()
}

#[no_mangle]
pub extern "C" fn hal_cpu_enumerate(topology: *mut hal_cpu_topology_t) -> HalStatus {
    if topology.is_null() {
        return HalError::InvalidArg.into();
    }
    crate::cpu::CpuSubsystem::enumerate(topology)
}

#[no_mangle]
pub extern "C" fn hal_cpu_get_info(cpu_id: u8, info: *mut hal_cpu_info_t) -> HalStatus {
    if info.is_null() {
        return HalError::InvalidArg.into();
    }
    crate::cpu::CpuSubsystem::get_info(cpu_id, info)
}

#[no_mangle]
pub extern "C" fn hal_cpu_has_feature(cpu_id: u8, feature: hal_cpu_feature_t) -> HalStatus {
    crate::cpu::CpuSubsystem::has_feature(cpu_id, feature)
}

#[no_mangle]
pub extern "C" fn hal_cpu_cpuid(
    cpu_id: u8,
    leaf: u32,
    subleaf: u32,
    result: *mut hal_cpuid_t,
) -> HalStatus {
    if result.is_null() {
        return HalError::InvalidArg.into();
    }
    crate::cpu::CpuSubsystem::cpuid(cpu_id, leaf, subleaf, result)
}

#[no_mangle]
pub extern "C" fn hal_cpu_read_msr(cpu_id: u8, msr: u32, value: *mut u64) -> HalStatus {
    if value.is_null() {
        return HalError::InvalidArg.into();
    }
    crate::cpu::CpuSubsystem::read_msr(cpu_id, msr, value)
}

#[no_mangle]
pub extern "C" fn hal_cpu_write_msr(cpu_id: u8, msr: u32, value: u64) -> HalStatus {
    crate::cpu::CpuSubsystem::write_msr(cpu_id, msr, value)
}

#[no_mangle]
pub extern "C" fn hal_cpu_read_cr(cpu_id: u8, reg: u8, value: *mut u64) -> HalStatus {
    if value.is_null() {
        return HalError::InvalidArg.into();
    }
    crate::cpu::CpuSubsystem::read_cr(cpu_id, reg, value)
}

#[no_mangle]
pub extern "C" fn hal_cpu_write_cr(cpu_id: u8, reg: u8, value: u64) -> HalStatus {
    crate::cpu::CpuSubsystem::write_cr(cpu_id, reg, value)
}

#[no_mangle]
pub extern "C" fn hal_cpu_send_ipi(mask: hal_cpu_mask_t, vector: u8) -> HalStatus {
    crate::cpu::CpuSubsystem::send_ipi(mask, vector)
}

#[no_mangle]
pub extern "C" fn hal_cpu_send_sipi(cpu_id: u8, target_addr: u32) -> HalStatus {
    crate::cpu::CpuSubsystem::send_sipi(cpu_id, target_addr)
}

#[no_mangle]
pub extern "C" fn hal_cpu_get_pstate(cpu_id: u8, pstate: *mut hal_cpu_pstate_t) -> HalStatus {
    if pstate.is_null() {
        return HalError::InvalidArg.into();
    }
    crate::cpu::CpuSubsystem::get_pstate(cpu_id, pstate)
}

#[no_mangle]
pub extern "C" fn hal_cpu_set_pstate(cpu_id: u8, pstate: u8) -> HalStatus {
    crate::cpu::CpuSubsystem::set_pstate(cpu_id, pstate)
}

#[no_mangle]
pub extern "C" fn hal_cpu_set_frequency(cpu_id: u8, frequency_mhz: u32) -> HalStatus {
    crate::cpu::CpuSubsystem::set_frequency(cpu_id, frequency_mhz)
}

#[no_mangle]
pub extern "C" fn hal_cpu_enable_turbo(cpu_id: u8) -> HalStatus {
    crate::cpu::CpuSubsystem::enable_turbo(cpu_id)
}

#[no_mangle]
pub extern "C" fn hal_cpu_disable_turbo(cpu_id: u8) -> HalStatus {
    crate::cpu::CpuSubsystem::disable_turbo(cpu_id)
}

#[no_mangle]
pub extern "C" fn hal_cpu_idle(cpu_id: u8, state: u8) -> HalStatus {
    crate::cpu::CpuSubsystem::idle(cpu_id, state)
}

#[no_mangle]
pub extern "C" fn hal_cpu_halt(cpu_id: u8) -> HalStatus {
    crate::cpu::CpuSubsystem::halt(cpu_id)
}

#[no_mangle]
pub extern "C" fn hal_cpu_pause() -> HalStatus {
    crate::cpu::CpuSubsystem::pause()
}

#[no_mangle]
pub extern "C" fn hal_cpu_get_stats(cpu_id: u8, stats: *mut hal_cpu_stats_t) -> HalStatus {
    if stats.is_null() {
        return HalError::InvalidArg.into();
    }
    crate::cpu::CpuSubsystem::get_stats(cpu_id, stats)
}

#[no_mangle]
pub extern "C" fn hal_cpu_get_cache_line_size() -> u32 {
    crate::cpu::CpuSubsystem::get_cache_line_size()
}

#[no_mangle]
pub extern "C" fn hal_cpu_get_tlb_entries(cpu_id: u8) -> u32 {
    crate::cpu::CpuSubsystem::get_tlb_entries(cpu_id)
}

#[no_mangle]
pub extern "C" fn hal_cpu_prefetch_enable(cpu_id: u8) -> HalStatus {
    crate::cpu::CpuSubsystem::prefetch_enable(cpu_id)
}

#[no_mangle]
pub extern "C" fn hal_cpu_prefetch_disable(cpu_id: u8) -> HalStatus {
    crate::cpu::CpuSubsystem::prefetch_disable(cpu_id)
}

#[no_mangle]
pub extern "C" fn hal_cpu_get_per_cpu_data(cpu_id: u8) -> *mut core::ffi::c_void {
    crate::cpu::CpuSubsystem::get_per_cpu_data(cpu_id)
}

#[no_mangle]
pub extern "C" fn hal_cpu_set_per_cpu_data(cpu_id: u8, data: *mut core::ffi::c_void) -> HalStatus {
    crate::cpu::CpuSubsystem::set_per_cpu_data(cpu_id, data)
}

#[no_mangle]
pub extern "C" fn hal_cpu_enable_virtualization(cpu_id: u8) -> HalStatus {
    crate::cpu::CpuSubsystem::enable_virtualization(cpu_id)
}

#[no_mangle]
pub extern "C" fn hal_cpu_disable_virtualization(cpu_id: u8) -> HalStatus {
    crate::cpu::CpuSubsystem::disable_virtualization(cpu_id)
}

#[no_mangle]
pub extern "C" fn hal_cpu_virtualization_enabled(cpu_id: u8) -> bool {
    crate::cpu::CpuSubsystem::virtualization_enabled(cpu_id)
}

pub const HAL_CPU_MASK_ALL: hal_cpu_mask_t = !0u64;
pub const HAL_CPU_MASK_NONE: hal_cpu_mask_t = 0u64;

pub const HAL_OK: HalStatus = 0;
pub const HAL_ERR_INVALID_ARG: HalStatus = -1;
pub const HAL_ERR_NOT_SUPPORTED: HalStatus = -2;
pub const HAL_ERR_DEVICE_BUSY: HalStatus = -3;
pub const HAL_ERR_NO_MEMORY: HalStatus = -4;
pub const HAL_ERR_TIMEOUT: HalStatus = -5;
pub const HAL_ERR_DEVICE_FAILED: HalStatus = -6;
pub const HAL_ERR_PERMISSION_DENIED: HalStatus = -7;
