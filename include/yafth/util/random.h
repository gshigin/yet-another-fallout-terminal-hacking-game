// Copyright 2024 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once

#include <bit>
#include <cstdint>
#include <array>
#include <limits>
#include <climits>

namespace yafth::util
{

constexpr auto seed(const uint64_t seed_) -> uint64_t
{
    constexpr uint64_t splitmix64_hex1 = 0x9e3779b97f4a7c15;
    constexpr uint64_t splitmix64_hex2 = 0xbf58476d1ce4e5b9;
    constexpr uint64_t splitmix64_hex3 = 0x94d049bb133111eb;

    constexpr uint64_t splitmix64_shift1 = 30;
    constexpr uint64_t splitmix64_shift2 = 27;
    constexpr uint64_t splitmix64_shift3 = 31;

    uint64_t shifted = seed_;

    // one splitmix64 iteration
    shifted += splitmix64_hex1;
    shifted = (shifted ^ (shifted >> splitmix64_shift1)) * splitmix64_hex2;
    shifted = (shifted ^ (shifted >> splitmix64_shift2)) * splitmix64_hex3;
    return shifted ^ (shifted >> splitmix64_shift3);
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

struct xoroshiro128
{
    std::array<uint64_t, 2> state;

    constexpr auto next() noexcept -> std::uint64_t
    {
        constexpr uint64_t xoroshiro128_shift1 = 17;
        constexpr uint64_t xoroshiro128_shift2 = 49;
        constexpr uint64_t xoroshiro128_shift3 = 21;
        constexpr uint64_t xoroshiro128_shift4 = 28;

        const auto s0 = state[0];
        auto s1 = state[1];
        const auto result = std::rotl(s0 + s1, xoroshiro128_shift1) + s0;

        s1 ^= s0;
        state[0] = std::rotl(s0, xoroshiro128_shift2) ^ s1 ^ (s1 << xoroshiro128_shift3);
        state[1] = std::rotl(s1, xoroshiro128_shift4);

        return result >> 4;
    }

    constexpr auto operator()() noexcept -> std::uint64_t
    {
        return next();
    }

    constexpr auto fork() noexcept -> xoroshiro128
    {
        return xoroshiro128{next(), next()};
    }

    static constexpr auto min() noexcept -> uint64_t { return 0;}
    static constexpr auto max() noexcept -> uint64_t { return std::numeric_limits<uint64_t>::max() >> 4;}
    using result_type = uint64_t;
};
} // namespace yafth::util