#ifndef YAFTH_RANDOM_GENERATOR_H
#define YAFTH_RANDOM_GENERATOR_H

#include <cstdint>

namespace yafth
{

    constexpr uint64_t seed(const uint64_t seed_)
    {
        uint64_t shifted = seed_;

        // one splitmix64 iteration
        shifted = (shifted += 0x9e3779b97f4a7c15);
        shifted = (shifted ^ (shifted >> 30)) * 0xbf58476d1ce4e5b9;
        shifted = (shifted ^ (shifted >> 27)) * 0x94d049bb133111eb;
        return shifted ^ (shifted >> 31);
    }

    // uses time of compilation for seeding rng
    // good for compile-time randomness
    constexpr uint64_t seed()
    {
        uint64_t shifted = 0;
        for(const auto c : __TIME__)
        {
            shifted <<= 8;
            shifted |= c;
        }

        return seed(shifted);
    }

    struct xoroshiro128
    {
        std::uint64_t state[2]{1, 1};

        xoroshiro128() = default;

        constexpr xoroshiro128(uint64_t s0, uint64_t s1) : state{s0, s1} {}

        // or c++20 std::rotl
        constexpr static inline uint64_t rotl(const uint64_t x, int k) 
        {
            return (x << k) | (x >> (64 - k));
        }

        constexpr std::uint64_t next() noexcept
        {
            const auto s0 = state[0];
            auto s1 = state[1];
            const auto result = rotl(s0 + s1, 17) + s0;

            s1 ^= s0;
            state[0] = rotl(s0, 49) ^ s1 ^ (s1 << 21);
            state[1] = rotl(s1, 28);

            return result >> 4;
        }

        template <typename T>
        constexpr T next_t() noexcept { return static_cast<T>(next()); }

        constexpr std::uint64_t operator()() noexcept { return next(); }

        constexpr xoroshiro128 fork() { return xoroshiro128{next(), next()}; }
    };
} // namespace yafth

#endif // YAFTH_RANDOM_GENERATOR_H