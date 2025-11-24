#include <kernel/process.h>

void x86_64_switch_context(thread_t *prev, thread_t *next)
{
    if (!next) return;
    
    asm volatile(
        "push %%rbp\n\t"
        "push %%rbx\n\t"
        "push %%r12\n\t"
        "push %%r13\n\t"
        "push %%r14\n\t"
        "push %%r15\n\t"
        
        "mov %%rsp, %0\n\t"
        "mov %1, %%rsp\n\t"
        
        "pop %%r15\n\t"
        "pop %%r14\n\t"
        "pop %%r13\n\t"
        "pop %%r12\n\t"
        "pop %%rbx\n\t"
        "pop %%rbp\n\t"
        
        : "=m"(prev->context.x86_64.rsp)
        : "m"(next->context.x86_64.rsp)
        : "memory"
    );
}

void x86_64_setup_thread_stack(thread_t *thread, void *entry_point, void *arg)
{
    u64 *stack = (u64 *)thread->user_stack;
    
    stack[0] = 0;
    stack[1] = (u64)entry_point;
    stack[2] = (u64)arg;
    
    thread->context.x86_64.rsp = (u64)(&stack[0]);
    thread->context.x86_64.rip = (u64)entry_point;
    thread->context.x86_64.rbp = (u64)(&stack[0]);
}

void x86_64_restore_context(x86_64_context_t *ctx)
{
    asm volatile(
        "mov %0, %%rax\n\t"
        "mov %1, %%rbx\n\t"
        "mov %2, %%rcx\n\t"
        "jmp *%3\n\t"
        :
        : "m"(ctx->rax), "m"(ctx->rbx), "m"(ctx->rcx), "m"(ctx->rip)
    );
}
