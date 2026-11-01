#ifndef COMMON_MATH_H
#define COMMON_MATH_H

#include <kernel/types.h>

u32 abs(s32 x);
u64 abs64(s64 x);
u32 min(u32 a, u32 b);
u32 max(u32 a, u32 b);
u64 min64(u64 a, u64 b);
u64 max64(u64 a, u64 b);
u32 pow2(u32 exp);
u32 log2(u32 x);
u64 gcd(u64 a, u64 b);
u64 lcm(u64 a, u64 b);

#endif

