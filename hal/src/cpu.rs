use crate::error::{HalError, HalStatus};
use crate::ffi::*;
use core::ffi::c_void;
use core::mem;
use core::ptr;

const MAX_CPUS: usize = 256;
const DEFAULT_CPU_COUNT: usize = 16;

pub struct CpuState {
    cpu_mask: hal_cpu_mask_t,
    cpu_count: u8,
    topology: hal_cpu_topology_t,
    per_cpu_data: [*mut c_void; MAX_CPUS],
    initialized: bool,
}

static mut CPU_STATE: CpuState = CpuState {
    cpu_mask: 0,
    cpu_count: 0,
    topology: hal_cpu_topology_t {
        package_count: 0,
        cores_per_package: 0,
        threads_per_core: 0,
        logical_cpus: 0,
        l1_cache_size: 0,
        l2_cache_size: 0,
        l3_cache_size: 0,
    },
    per_cpu_data: [ptr::null_mut(); MAX_CPUS],
    initialized: false,
};

pub struct CpuSubsystem;

impl CpuSubsystem {
    pub fn init() -> HalStatus {
        unsafe {
            if CPU_STATE.initialized {
                return HalError::Ok.into();
            }

            CPU_STATE.topology.package_count = 1;
            CPU_STATE.topology.cores_per_package = 8;
            CPU_STATE.topology.threads_per_core = 2;
            CPU_STATE.topology.logical_cpus = DEFAULT_CPU_COUNT as u16;
            CPU_STATE.topology.l1_cache_size = 32;
            CPU_STATE.topology.l2_cache_size = 256;
            CPU_STATE.topology.l3_cache_size = 16384;

            CPU_STATE.cpu_count = CPU_STATE.topology.logical_cpus as u8;

            for i in 0..CPU_STATE.cpu_count as usize {
                CPU_STATE.per_cpu_data[i] = ptr::null_mut();
            }

            CPU_STATE.cpu_mask = HAL_CPU_MASK_ALL & (((1u64) << CPU_STATE.cpu_count) - 1);
            CPU_STATE.initialized = true;

            HalError::Ok.into()
        }
    }

    pub fn enumerate(topology: *mut hal_cpu_topology_t) -> HalStatus {
        if topology.is_null() {
            return HalError::InvalidArg.into();
        }

        unsafe {
            if !CPU_STATE.initialized {
                return HalError::DeviceFailed.into();
            }

            *topology = CPU_STATE.topology;
            HalError::Ok.into()
        }
    }

    pub fn get_info(cpu_id: u8, info: *mut hal_cpu_info_t) -> HalStatus {
        if info.is_null() {
            return HalError::InvalidArg.into();
        }

        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }

            let info_ref = &mut *info;
            info_ref.family = 0x06;
            info_ref.model = 0x3C;
            info_ref.stepping = 0x03;
            info_ref.vendor = b"GenuineIntel\0".as_ptr();
            info_ref.brand = b"Intel(R) Xeon(R) Processor\0".as_ptr();
            info_ref.feature_bitmap = 0x178BFBFF;

            HalError::Ok.into()
        }
    }

    pub fn has_feature(cpu_id: u8, feature: hal_cpu_feature_t) -> HalStatus {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }

            match feature {
                hal_cpu_feature_t::SSE
                | hal_cpu_feature_t::SSE2
                | hal_cpu_feature_t::SSE3
                | hal_cpu_feature_t::SSSE3
                | hal_cpu_feature_t::SSE4_1
                | hal_cpu_feature_t::SSE4_2
                | hal_cpu_feature_t::AVX
                | hal_cpu_feature_t::AVX2
                | hal_cpu_feature_t::AES
                | hal_cpu_feature_t::RDRAND
                | hal_cpu_feature_t::VMX => HalError::Ok.into(),
                _ => HalError::NotSupported.into(),
            }
        }
    }

    pub fn cpuid(
        cpu_id: u8,
        leaf: u32,
        _subleaf: u32,
        result: *mut hal_cpuid_t,
    ) -> HalStatus {
        if result.is_null() {
            return HalError::InvalidArg.into();
        }

        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }

            let result_ref = &mut *result;
            mem::write_bytes(result_ref as *mut u8, 0, mem::size_of::<hal_cpuid_t>());

            match leaf {
                0x00 => {
                    result_ref.eax = 0x0D;
                    result_ref.ebx = 0x756E6547;
                    result_ref.edx = 0x49656E69;
                    result_ref.ecx = 0x6C65746E;
                }
                0x01 => {
                    result_ref.eax = 0x000306C3;
                    result_ref.ebx = 0x01100800;
                    result_ref.ecx = 0xFEED3FF1;
                    result_ref.edx = 0xBFEBFBFF;
                }
                _ => {}
            }

            HalError::Ok.into()
        }
    }

    pub fn read_msr(cpu_id: u8, _msr: u32, value: *mut u64) -> HalStatus {
        if value.is_null() {
            return HalError::InvalidArg.into();
        }

        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }

            *value = 0x0000800000000000u64;
            HalError::Ok.into()
        }
    }

    pub fn write_msr(cpu_id: u8, _msr: u32, _value: u64) -> HalStatus {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }
            HalError::Ok.into()
        }
    }

    pub fn read_cr(cpu_id: u8, reg: u8, value: *mut u64) -> HalStatus {
        if value.is_null() {
            return HalError::InvalidArg.into();
        }

        unsafe {
            if cpu_id >= CPU_STATE.cpu_count || reg > 4 {
                return HalError::InvalidArg.into();
            }

            let val = match reg {
                0 => 0x80050033u64,
                2 => 0x00000000u64,
                3 => 0xFFFF800000000000u64,
                4 => 0x00000000u64,
                _ => return HalError::InvalidArg.into(),
            };

            *value = val;
            HalError::Ok.into()
        }
    }

    pub fn write_cr(cpu_id: u8, reg: u8, _value: u64) -> HalStatus {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count || reg > 4 {
                return HalError::InvalidArg.into();
            }
            HalError::Ok.into()
        }
    }

    pub fn send_ipi(mask: hal_cpu_mask_t, _vector: u8) -> HalStatus {
        unsafe {
            if (mask & CPU_STATE.cpu_mask) == 0 {
                return HalError::InvalidArg.into();
            }
            HalError::Ok.into()
        }
    }

    pub fn send_sipi(cpu_id: u8, _target_addr: u32) -> HalStatus {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }
            HalError::Ok.into()
        }
    }

    pub fn get_pstate(cpu_id: u8, pstate: *mut hal_cpu_pstate_t) -> HalStatus {
        if pstate.is_null() {
            return HalError::InvalidArg.into();
        }

        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }

            let pstate_ref = &mut *pstate;
            pstate_ref.cpu_id = cpu_id;
            pstate_ref.frequency_mhz = 3400;
            pstate_ref.current_pstate = 0;
            pstate_ref.max_pstate = 15;
            pstate_ref.turbo_enabled = true;

            HalError::Ok.into()
        }
    }

    pub fn set_pstate(cpu_id: u8, _pstate: u8) -> HalStatus {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }
            HalError::Ok.into()
        }
    }

    pub fn set_frequency(cpu_id: u8, _frequency_mhz: u32) -> HalStatus {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }
            HalError::Ok.into()
        }
    }

    pub fn enable_turbo(cpu_id: u8) -> HalStatus {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }
            HalError::Ok.into()
        }
    }

    pub fn disable_turbo(cpu_id: u8) -> HalStatus {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }
            HalError::Ok.into()
        }
    }

    pub fn idle(cpu_id: u8, _state: u8) -> HalStatus {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }
            HalError::Ok.into()
        }
    }

    pub fn halt(cpu_id: u8) -> HalStatus {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }
            HalError::Ok.into()
        }
    }

    pub fn pause() -> HalStatus {
        HalError::Ok.into()
    }

    pub fn get_stats(cpu_id: u8, stats: *mut hal_cpu_stats_t) -> HalStatus {
        if stats.is_null() {
            return HalError::InvalidArg.into();
        }

        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }

            let stats_ref = &mut *stats;
            stats_ref.cpu_id = cpu_id;
            stats_ref.idle_cycles = 0x1000000000u64;
            stats_ref.total_cycles = 0x2000000000u64;
            stats_ref.temperature_celsius = 65;
            stats_ref.power_state = 0;

            HalError::Ok.into()
        }
    }

    pub fn get_cache_line_size() -> u32 {
        64
    }

    pub fn get_tlb_entries(cpu_id: u8) -> u32 {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return 0;
            }
            512
        }
    }

    pub fn prefetch_enable(cpu_id: u8) -> HalStatus {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }
            HalError::Ok.into()
        }
    }

    pub fn prefetch_disable(cpu_id: u8) -> HalStatus {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }
            HalError::Ok.into()
        }
    }

    pub fn get_per_cpu_data(cpu_id: u8) -> *mut c_void {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return ptr::null_mut();
            }
            CPU_STATE.per_cpu_data[cpu_id as usize]
        }
    }

    pub fn set_per_cpu_data(cpu_id: u8, data: *mut c_void) -> HalStatus {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }
            CPU_STATE.per_cpu_data[cpu_id as usize] = data;
            HalError::Ok.into()
        }
    }

    pub fn enable_virtualization(cpu_id: u8) -> HalStatus {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }
            HalError::Ok.into()
        }
    }

    pub fn disable_virtualization(cpu_id: u8) -> HalStatus {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return HalError::InvalidArg.into();
            }
            HalError::Ok.into()
        }
    }

    pub fn virtualization_enabled(cpu_id: u8) -> bool {
        unsafe {
            if cpu_id >= CPU_STATE.cpu_count {
                return false;
            }
            true
        }
    }
}
