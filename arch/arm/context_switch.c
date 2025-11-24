#include <kernel/process.h>

void arm_switch_context(thread_t *prev, thread_t *next)
{
    if (!next) return;
    
    asm volatile(
        "stp x29, x30, [sp, #-16]!\n\t"
        "stp x27, x28, [sp, #-16]!\n\t"
        "stp x25, x26, [sp, #-16]!\n\t"
        "stp x23, x24, [sp, #-16]!\n\t"
        
        "mov sp, %0\n\t"
        "mov sp, %1\n\t"
        
        "ldp x23, x24, [sp], #16\n\t"
        "ldp x25, x26, [sp], #16\n\t"
        "ldp x27, x28, [sp], #16\n\t"
        "ldp x29, x30, [sp], #16\n\t"
        
        : "=r"(prev->context.arm.sp)
        : "r"(next->context.arm.sp)
        : "memory"
    );
}

void arm_setup_thread_stack(thread_t *thread, void *entry_point, void *arg)
{
    u64 *stack = (u64 *)thread->user_stack;
    
    stack[0] = 0;
    stack[1] = (u64)entry_point;
    stack[2] = (u64)arg;
    
    thread->context.arm.sp = (u32)(&stack[0]);
    thread->context.arm.pc = (u32)entry_point;
    thread->context.arm.r15 = (u32)entry_point;
}

void arm_restore_context(arm_context_t *ctx)
{
    asm volatile(
        "mov x0, %0\n\t"
        "mov x1, %1\n\t"
        "mov x2, %2\n\t"
        "br %3\n\t"
        :
        : "r"(ctx->r0), "r"(ctx->r1), "r"(ctx->r2), "r"(ctx->pc)
    );
}
