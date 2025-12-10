// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
// yafth
#include "game_state.hpp"
// stl
#include <cassert>

namespace yafth::core::detail
{
auto game_state::is_bracket_used(size_t index) const noexcept -> bool
{
    assert(index < used_brackets_.size());

    return used_brackets_.test(index);
}

void game_state::mark_bracket_used(size_t index) noexcept
{
    assert(index < used_brackets_.size());

    used_brackets_.set(index);
}

auto game_state::attempts_left() const noexcept -> size_t
{
    return static_cast<size_t>(attempts_left_);
}

auto game_state::consume_attempt() noexcept -> bool
{
    if (attempts_left_ == 0)
    {
        return false;
    }

    --attempts_left_;
    return true;
}

void game_state::reset_attempts() noexcept
{
    attempts_left_ = 4;
}

auto game_state::is_game_over() const noexcept -> bool
{
    return game_over_;
}

void game_state::set_game_over() noexcept
{
    game_over_ = true;
}
} // namespace yafth::core::detail