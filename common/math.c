#include <kernel/types.h>

u32 abs(s32 x)
{
    return x < 0 ? -x : x;
}

u64 abs64(s64 x)
{
    return x < 0 ? -x : x;
}

u32 min(u32 a, u32 b)
{
    return a < b ? a : b;
}

u32 max(u32 a, u32 b)
{
    return a > b ? a : b;
}

u64 min64(u64 a, u64 b)
{
    return a < b ? a : b;
}

u64 max64(u64 a, u64 b)
{
    return a > b ? a : b;
}

u32 pow2(u32 exp)
{
    return 1 << exp;
}

u32 log2(u32 x)
{
    u32 result = 0;
    while ((1U << result) < x) result++;
    return result;
}

u64 gcd(u64 a, u64 b)
{
    while (b) {
        u64 temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

u64 lcm(u64 a, u64 b)
{
    return (a / gcd(a, b)) * b;
}
