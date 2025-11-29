#ifndef AEGIS_ARM_BOOT_H
#define AEGIS_ARM_BOOT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define ARM32_MAGIC_ZERO 0x00000000
#define ARM32_MAGIC_PSCI 0x65726f43
#define ARM64_MAGIC 0x644d5241
#define PSCI_VERSION_0_2 0x00000002

typedef enum {
    ARM_BOOT_MODE_32BIT = 1,
    ARM_BOOT_MODE_64BIT = 2,
    ARM_BOOT_MODE_THUMB = 3
} arm_boot_mode_t;

typedef enum {
    ARM_EXCEPTION_MODE_SVC = 0x13,
    ARM_EXCEPTION_MODE_FIQ = 0x11,
    ARM_EXCEPTION_MODE_IRQ = 0x12,
    ARM_EXCEPTION_MODE_ABT = 0x17,
    ARM_EXCEPTION_MODE_UND = 0x1B,
    ARM_EXCEPTION_MODE_SYS = 0x1F
} arm_exception_mode_t;

typedef enum {
    ARM_PROCESSOR_CORTEX_A8 = 0xC08,
    ARM_PROCESSOR_CORTEX_A9 = 0xC09,
    ARM_PROCESSOR_CORTEX_A15 = 0xC0F,
    ARM_PROCESSOR_CORTEX_A53 = 0xD03,
    ARM_PROCESSOR_CORTEX_A57 = 0xD07,
    ARM_PROCESSOR_CORTEX_A72 = 0xD08,
    ARM_PROCESSOR_CORTEX_A73 = 0xD09,
    ARM_PROCESSOR_CORTEX_A75 = 0xD0A
} arm_processor_id_t;

typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t sp;
    uint32_t lr;
    uint32_t pc;
    uint32_t cpsr;
} arm32_registers_t;

typedef struct {
    uint64_t x0;
    uint64_t x1;
    uint64_t x2;
    uint64_t x3;
    uint64_t x4;
    uint64_t x5;
    uint64_t x6;
    uint64_t x7;
    uint64_t x8;
    uint64_t x30;
    uint64_t sp;
    uint64_t pc;
    uint64_t pstate;
} arm64_registers_t;

typedef struct {
    arm_boot_mode_t mode;
    uint32_t cpu_id;
    uint32_t boot_processor_cpuid;
    uint64_t machine_type;
    uint64_t device_tree_addr;
    uint32_t flags;
    uint32_t processor_id;
    uint32_t compat_id;
} arm_boot_info_t;

typedef struct {
    uint32_t cache_level;
    uint32_t cache_type;
    uint32_t cache_size;
    uint32_t cache_line_size;
    uint32_t ways;
    uint32_t sets;
} arm_cache_info_t;

typedef struct {
    uint32_t vbar_el1;
    uint32_t vbar_el2;
    uint32_t vbar_el3;
    uint64_t exception_vectors;
} arm_exception_vectors_t;

typedef struct {
    uint32_t ttbr0_flags;
    uint32_t ttbr1_flags;
    uint32_t tcr_flags;
    uint32_t mair_flags;
    uint64_t translation_table;
    bool is_64bit_ptrs;
} arm_mmu_config_t;

typedef struct {
    uint32_t sctlr;
    uint32_t actlr;
    uint32_t cpacr;
    uint32_t nsacr;
    uint32_t htcr;
    uint32_t vtcr;
    bool has_security_extensions;
    bool has_virtualization;
    bool has_lpae;
} arm32_control_registers_t;

typedef struct {
    uint64_t sctlr_el1;
    uint64_t sctlr_el2;
    uint64_t sctlr_el3;
    uint64_t hcr_el2;
    uint64_t scr_el3;
    uint64_t tcr_el1;
    uint64_t tcr_el2;
    uint64_t tcr_el3;
} arm64_control_registers_t;

arm_boot_info_t *arm_init_boot_info(void);
bool arm_validate_device_tree(const void *dtb, uint64_t size);
int arm_load_device_tree_blob(const void *dtb, uint64_t load_addr);
bool arm_setup_exception_vectors(arm_exception_vectors_t *vectors);
bool arm_configure_mmu_32bit(arm_mmu_config_t *config);
bool arm_configure_mmu_64bit(arm_mmu_config_t *config);
bool arm_enable_caches(uint32_t cache_config);
bool arm_enable_tlb(void);
bool arm_setup_security_extensions(void);
bool arm_setup_virtualization(void);
int arm_detect_processor(void);
int arm_get_cache_info(arm_cache_info_t *info);
bool arm_flush_tlb_all(void);
bool arm_flush_cache_all(void);
uint32_t arm32_get_cpuid(void);
uint64_t arm64_get_mpidr(void);
bool arm_is_64bit(void);
bool arm_is_32bit(void);
bool arm_has_lpae(void);
int arm_get_core_count(void);
uint32_t arm_get_cpu_id(void);

#endif
