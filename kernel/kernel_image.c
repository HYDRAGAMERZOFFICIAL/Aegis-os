#include <kernel/types.h>

extern int kernel_main(void);
extern void setup_arch(void);

int main(void)
{
    setup_arch();
    return kernel_main();
}
