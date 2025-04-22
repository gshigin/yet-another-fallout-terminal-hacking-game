// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once

#include <climits>
#include <cstdint>
#include <limits>

namespace yafth::util
{

constexpr auto seed(const uint64_t seed_) -> uint64_t
{
    uint64_t shifted = seed_;

    // one splitmix64 iteration
    shifted += 0x9e3779b97f4a7c15;
    shifted = (shifted ^ (shifted >> 30)) * 0xbf58476d1ce4e5b9;
    shifted = (shifted ^ (shifted >> 27)) * 0x94d049bb133111eb;
    return shifted ^ (shifted >> 31);
}

// uses time of compilation for seeding rng
constexpr auto seed_time() -> uint64_t
{
    uint64_t shifted = 0;
    for (char c : __TIME__)
    {
        shifted <<= CHAR_BIT;
        shifted |= c;
    }

    return seed(shifted);
}

struct xorshift32
{
    uint32_t state;

    constexpr auto next() noexcept -> uint32_t
    {
        uint32_t x = state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return (state = x);
    }

    constexpr auto operator()() noexcept -> uint32_t
    {
        return next();
    }

    constexpr auto fork() noexcept -> xorshift32
    {
        return xorshift32{next()};
    }

    static constexpr auto min() noexcept -> uint32_t
    {
        return 0;
    }
    static constexpr auto max() noexcept -> uint32_t
    {
        return std::numeric_limits<uint32_t>::max();
    }
    using result_type = uint32_t;
};

} // namespace yafth::util