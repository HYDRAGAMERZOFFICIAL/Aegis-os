#ifndef COMMON_BITMAP_H
#define COMMON_BITMAP_H

#include <kernel/types.h>

void bitmap_set(u8 *bitmap, u32 bit);
void bitmap_clear(u8 *bitmap, u32 bit);
int bitmap_test(const u8 *bitmap, u32 bit);
u32 bitmap_find_first_clear(const u8 *bitmap, u32 size);
u32 bitmap_find_first_set(const u8 *bitmap, u32 size);
u32 bitmap_count_set(const u8 *bitmap, u32 size);

#endif

