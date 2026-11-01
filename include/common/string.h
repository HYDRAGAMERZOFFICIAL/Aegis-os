#ifndef COMMON_STRING_H
#define COMMON_STRING_H

#include <kernel/types.h>
#include <stddef.h>

int strcmp(const char *s1, const char *s2);
size_t strlen(const char *s);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

#endif
