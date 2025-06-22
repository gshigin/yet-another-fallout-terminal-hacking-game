// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once
// yafth
#include <yafth/core/types.h>
// stl
#include <functional>
#include <random>

// fwd
namespace yafth::core::detail
{
class game_state;
class terminal_buffer;
class word_repository;
} // namespace yafth::core::detail

namespace yafth::core::detail::interaction_logic
{
namespace _internal
{
using rng_fref = const std::function<uint32_t()> &;

auto click_at(std::size_t index, terminal_buffer &terminal, word_repository &words, game_state &state, rng_fref gen_next) noexcept -> click_status;
template <class URBG>
    requires std::uniform_random_bit_generator<std::remove_reference_t<URBG>>
constexpr auto wrap_rng(URBG &&rng) noexcept;
} // namespace _internal

auto look_at(std::size_t index, const terminal_buffer &terminal, game_state &state) noexcept -> highlight;

template <class URBG>
    requires std::uniform_random_bit_generator<std::remove_reference_t<URBG>>
auto click_at(std::size_t index, terminal_buffer &terminal, word_repository &words, game_state &state, URBG &g) noexcept -> click_status
{
    return _internal::click_at(index, terminal, words, state, _internal::wrap_rng(std::forward<URBG>(g)));
}

} // namespace yafth::core::detail::interaction_logic

namespace yafth::core::detail::interaction_logic::_internal
{
template <class URBG>
    requires std::uniform_random_bit_generator<std::remove_reference_t<URBG>>
constexpr auto wrap_rng(URBG &&rng) noexcept
{
    return [&]() { return rng(); };
}
} // namespace yafth::core::detail::interaction_logic::_internal