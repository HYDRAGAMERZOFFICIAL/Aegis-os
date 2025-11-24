#ifndef AEGIS_KERNEL_TYPES_H
#define AEGIS_KERNEL_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef unsigned long ulong;
typedef unsigned int uint;

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12
#define PAGE_MASK (~(PAGE_SIZE - 1))

#define KERNEL_BASE 0xFFFF800000000000UL
#define KERNEL_HEAP_BASE 0xFFFF800000100000UL

typedef struct {
    u64 id;
    const char *name;
} kernel_subsystem_t;

typedef int (*handler_t)(void *arg);
typedef void (*irq_handler_t)(u32 irq);

#define MAX_CPUS 256
#define MAX_PROCESSES 4096
#define MAX_THREADS_PER_PROCESS 256
#define MAX_IRQ_HANDLERS 256

#endif
