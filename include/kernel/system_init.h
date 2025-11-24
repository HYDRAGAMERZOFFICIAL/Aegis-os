#ifndef __SYSTEM_INIT_H__
#define __SYSTEM_INIT_H__

int system_init(void);
int system_shutdown(void);
int system_is_initialized(void);
void printk(const char* fmt, ...);

#endif
