#include <string.h>
#include <kernel/types.h>

int strcmp(const char *s1, const char *s2)
{
    while (*s1 == *s2++) {
        if (*s1++ == '\0') return 0;
    }
    return *(const u8 *)s1 - *(const u8 *)--s2;
}

size_t strlen(const char *s)
{
    size_t len = 0;
    while (s[len] != '\0') len++;
    return len;
}

char *strcpy(char *dst, const char *src)
{
    while ((*dst++ = *src++) != '\0');
    return dst - strlen(src) - 1;
}

char *strncpy(char *dst, const char *src, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        dst[i] = src[i];
        if (src[i] == '\0') break;
    }
    return dst;
}

void *memset(void *s, int c, size_t n)
{
    u8 *p = (u8 *)s;
    for (size_t i = 0; i < n; i++) {
        p[i] = (u8)c;
    }
    return s;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    u8 *d = (u8 *)dst;
    const u8 *s = (const u8 *)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dst;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const u8 *p1 = (const u8 *)s1;
    const u8 *p2 = (const u8 *)s2;
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) return p1[i] - p2[i];
    }
    return 0;
}
