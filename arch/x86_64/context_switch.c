#include <kernel/process.h>
#include <kernel/types.h>
#include <string.h>

/*
 * x86-64 Context Switching and Thread Execution Environment
 * Compliant with AMD64 System V ABI.
 */

extern void x86_64_set_tss_rsp0(u64 rsp0);

static thread_t *current_running_thread = NULL;

thread_t *x86_64_get_current_thread(void)
{
    return current_running_thread;
}

void x86_64_set_current_thread(thread_t *thread)
{
    current_running_thread = thread;
}

#if defined(__x86_64__) && !defined(_WIN32)
/*
 * Bare-metal context switch routine.
 * Saves callee-saved registers (RBX, RBP, R12-R15) and RFLAGS,
 * switches RSP, and restores next thread's state.
 */
__attribute__((naked)) void x86_64_cpu_switch(u64 *prev_rsp, u64 next_rsp)
{
    asm volatile(
        "pushq %rbp\n\t"
        "pushq %rbx\n\t"
        "pushq %r12\n\t"
        "pushq %r13\n\t"
        "pushq %r14\n\t"
        "pushq %r15\n\t"
        "pushfq\n\t"
        "movq %%rsp, (%rdi)\n\t"
        "movq %rsi, %%rsp\n\t"
        "popfq\n\t"
        "popq %r15\n\t"
        "popq %r14\n\t"
        "popq %r13\n\t"
        "popq %r12\n\t"
        "popq %rbx\n\t"
        "popq %rbp\n\t"
        "retq\n\t"
    );
}
#endif

void x86_64_switch_context(thread_t *prev, thread_t *next)
{
    if (!next) return;

    if (prev) {
        prev->state = PROCESS_STATE_READY;
    }
    next->state = PROCESS_STATE_RUNNING;
    current_running_thread = next;

    // Update TSS RSP0 to this thread's kernel stack top for ring 3->0 transitions
    if (next->kernel_stack) {
        u64 stack_top = (u64)(uintptr_t)next->kernel_stack + (2 * 4096);
        x86_64_set_tss_rsp0(stack_top);
    }

#if defined(__x86_64__) && !defined(_WIN32)
    if (prev) {
        x86_64_cpu_switch(&prev->context.x86_64.rsp, next->context.x86_64.rsp);
    } else {
        // First thread execution: load stack pointer and jump to entry
        asm volatile(
            "movq %0, %%rsp\n\t"
            "popfq\n\t"
            "popq %%r15\n\t"
            "popq %%r14\n\t"
            "popq %%r13\n\t"
            "popq %%r12\n\t"
            "popq %%rbx\n\t"
            "popq %%rbp\n\t"
            "retq\n\t"
            :
            : "r"(next->context.x86_64.rsp)
            : "memory"
        );
    }
#else
    // Host / simulation mode: safely track simulated CPU context registers
    if (prev) {
        // Save virtual registers
        prev->context.x86_64.rflags = 0x202; // IF set
    }
    next->context.x86_64.rflags = 0x202;
#endif
}

void x86_64_setup_thread_stack(thread_t *thread, void *entry_point, void *arg)
{
    if (!thread || !entry_point) return;

    // Determine kernel stack boundaries (stack grows downwards)
    u64 stack_top;
    if (thread->kernel_stack) {
        stack_top = (u64)(uintptr_t)thread->kernel_stack + (2 * 4096);
    } else if (thread->user_stack) {
        stack_top = (u64)(uintptr_t)thread->user_stack + (4 * 4096);
    } else {
        return;
    }

    // Align to 16 bytes per System V ABI
    stack_top &= ~0xFULL;

    u64 *stack = (u64 *)(uintptr_t)stack_top;

    // Stack layout expected by switch context return:
    // [stack - 1] = entry_point (return address for retq)
    // [stack - 2] = RFLAGS (0x202 - IF enabled)
    // [stack - 3] = R15 (0)
    // [stack - 4] = R14 (0)
    // [stack - 5] = R13 (0)
    // [stack - 6] = R12 (0)
    // [stack - 7] = RBX (0)
    // [stack - 8] = RBP (0)
    
    stack[-1] = (u64)(uintptr_t)entry_point;
    stack[-2] = 0x202; // RFLAGS
    stack[-3] = 0;     // R15
    stack[-4] = 0;     // R14
    stack[-5] = 0;     // R13
    stack[-6] = 0;     // R12
    stack[-7] = 0;     // RBX
    stack[-8] = 0;     // RBP

    u64 initial_rsp = (u64)(uintptr_t)&stack[-8];

    thread->context.x86_64.rsp = initial_rsp;
    thread->context.x86_64.rbp = initial_rsp;
    thread->context.x86_64.rip = (u64)(uintptr_t)entry_point;
    thread->context.x86_64.rdi = (u64)(uintptr_t)arg; // System V ABI: 1st argument in RDI
    thread->context.x86_64.rflags = 0x202;
    thread->context.x86_64.rax = 0;
    thread->context.x86_64.rbx = 0;
    thread->context.x86_64.rcx = 0;
    thread->context.x86_64.rdx = 0;
    thread->context.x86_64.rsi = 0;
}

void x86_64_restore_context(x86_64_context_t *ctx)
{
    if (!ctx) return;

#if defined(__x86_64__) && !defined(_WIN32)
    asm volatile(
        "movq %0, %%rsp\n\t"
        "movq %1, %%rax\n\t"
        "movq %2, %%rbx\n\t"
        "movq %3, %%rcx\n\t"
        "movq %4, %%rdx\n\t"
        "movq %5, %%rsi\n\t"
        "movq %6, %%rdi\n\t"
        "jmpq *%7\n\t"
        :
        : "m"(ctx->rsp), "m"(ctx->rax), "m"(ctx->rbx), "m"(ctx->rcx),
          "m"(ctx->rdx), "m"(ctx->rsi), "m"(ctx->rdi), "m"(ctx->rip)
        : "memory"
    );
#else
    // Host / simulation mode: record restored state
#endif
}
