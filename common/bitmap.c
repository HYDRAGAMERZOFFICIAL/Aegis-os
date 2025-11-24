#include <kernel/types.h>
#include <string.h>

void bitmap_set(u8 *bitmap, u32 bit)
{
    u32 byte_idx = bit / 8;
    u32 bit_idx = bit % 8;
    bitmap[byte_idx] |= (1 << bit_idx);
}

void bitmap_clear(u8 *bitmap, u32 bit)
{
    u32 byte_idx = bit / 8;
    u32 bit_idx = bit % 8;
    bitmap[byte_idx] &= ~(1 << bit_idx);
}

int bitmap_test(const u8 *bitmap, u32 bit)
{
    u32 byte_idx = bit / 8;
    u32 bit_idx = bit % 8;
    return (bitmap[byte_idx] >> bit_idx) & 1;
}

u32 bitmap_find_first_clear(const u8 *bitmap, u32 size)
{
    for (u32 i = 0; i < size * 8; i++) {
        if (!bitmap_test(bitmap, i)) {
            return i;
        }
    }
    return size * 8;
}

u32 bitmap_find_first_set(const u8 *bitmap, u32 size)
{
    for (u32 i = 0; i < size * 8; i++) {
        if (bitmap_test(bitmap, i)) {
            return i;
        }
    }
    return size * 8;
}

u32 bitmap_count_set(const u8 *bitmap, u32 size)
{
    u32 count = 0;
    for (u32 i = 0; i < size * 8; i++) {
        if (bitmap_test(bitmap, i)) {
            count++;
        }
    }
    return count;
}
