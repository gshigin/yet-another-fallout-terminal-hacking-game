// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once
// stl
#include <optional>
#include <string_view>

namespace yafth::inline types
{
enum class lock_level : uint8_t
{
    very_easy,
    easy,
    average,
    hard,
    very_hard
};

enum class input_type : uint8_t
{
    look,
    click,
    other // treated as nothing happened
};

struct screen_coords
{
    int x;
    int y;
};

struct input
{
    input_type type;
    std::optional<screen_coords> coords;
};

struct highlight
{
    std::size_t begin;
    std::size_t end;
};

enum class click_result
{
    error,                 // when clicking free symbol
    dud_removed,           // when clicked brackets, removes one wrong answer
    allowance_replenished, // when clicked brackets, restores attempts
    entry_denied,          // when clicked wrong answer
    lockout_in_progress,   // lock after all attemps are over
    exact_match            // when guessed the answer
};

struct word_match
{
    std::size_t of;
    std::size_t from;
};

struct click_status
{
    click_result state;
    std::optional<word_match> match;
};

struct state
{
    std::string_view term_chars; // all current mutable characters
    std::size_t attempts_left;
    std::optional<highlight> highlighted;
    std::optional<click_status> click_res; // if click occured
};

struct args
{
    std::uint32_t science_level;
    lock_level lock;
    std::uint64_t seed;
};

} // namespace yafth::inline types