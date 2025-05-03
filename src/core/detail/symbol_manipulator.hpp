// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once
// stl
#include <array>
#include <functional>
#include <string>

// fwd
namespace yafth::core::detail
{
class terminal_buffer;
}

namespace yafth::core::detail::symbol_manipulator
{
// fwd
namespace _internal
{
using rng_fref = const std::function<uint32_t()> &;

auto generate_words(std::size_t length, std::size_t count, rng_fref gen_next) noexcept -> std::array<std::string, 20>;
void generate_term_chars(terminal_buffer &terminal, rng_fref gen_next) noexcept;
auto place_words(terminal_buffer &terminal, const std::array<std::string, 20> &words, std::size_t length, std::size_t count,
                 rng_fref gen_next) noexcept -> std::array<uint16_t, 20>;

template <class URBG>
    requires std::uniform_random_bit_generator<std::remove_reference_t<URBG>>
constexpr auto wrap_rng(URBG &&rng) noexcept;

} // namespace _internal

template <class URBG>
    requires std::uniform_random_bit_generator<std::remove_reference_t<URBG>>
auto generate_words(std::size_t length, std::size_t count, URBG &&g) noexcept -> std::array<std::string, 20>
{
    return _internal::generate_words(length, count, _internal::wrap_rng(std::forward<URBG>(g)));
}

template <class URBG>
    requires std::uniform_random_bit_generator<std::remove_reference_t<URBG>>
void generate_term_chars(terminal_buffer &terminal, URBG &&g) noexcept
{
    return _internal::generate_term_chars(terminal, _internal::wrap_rng(std::forward<URBG>(g)));
}

template <class URBG>
    requires std::uniform_random_bit_generator<std::remove_reference_t<URBG>>
auto place_words(terminal_buffer &terminal, const std::array<std::string, 20> &words, std::size_t length, std::size_t count,
                 URBG &&g) noexcept -> std::array<uint16_t, 20>
{
    return _internal::place_words(terminal, words, length, count, _internal::wrap_rng(std::forward<URBG>(g)));
}

} // namespace yafth::core::detail::symbol_manipulator

namespace yafth::core::detail::symbol_manipulator::_internal
{
template <class URBG>
    requires std::uniform_random_bit_generator<std::remove_reference_t<URBG>>
constexpr auto wrap_rng(URBG &&rng) noexcept
{
    return [&]() { return rng(); };
}
} // namespace yafth::core::detail::symbol_manipulator::_internal