// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
// yafth
#include "detail/game_state.hpp"
#include "detail/interaction_logic.hpp"
#include "detail/symbol_manipulator.hpp"
#include "detail/terminal_buffer.hpp"
#include "detail/word_repository.hpp"
#include <yafth/core/engine.h>
#include <yafth/core/types.h>
#include <yafth/util/random.h>
// stl
#include <algorithm>
#include <cassert>
#include <string>
#include <utility>

namespace
{
// constexpr brackets lookup
static constexpr std::array<std::pair<char, char>, 4> brackets{{{'{', '}'}, {'(', ')'}, {'<', '>'}, {'[', ']'}}};

constexpr auto bracket_lookup = [](const char c) { return std::find_if(begin(brackets), end(brackets), [&c](const auto &v) { return v.first == c; })->second; };

constexpr auto is_char = [](const char c) { return c >= 'A' && c <= 'Z'; };
constexpr auto is_open_br = [](const char c) { return c == '(' || c == '[' || c == '<' || c == '{'; };

// from https://geckwiki.com/index.php?title=Hacking_Word_Count
auto compute_word_count(yafth::lock_level lock_level_setting, uint32_t player_science_skill) noexcept -> std::size_t
{
    assert(0 <= player_science_skill && player_science_skill <= 100);

    constexpr std::size_t hacking_min_words = 5;
    constexpr std::size_t hacking_max_words = 20;
    const std::size_t lock_level = static_cast<std::size_t>(lock_level_setting) * 25;
    const std::size_t science_offset = player_science_skill - lock_level;
    const std::size_t lock_offset = 100 - lock_level;

    const double sol_coef =
        lock_offset == 0 ? 0.5 : (1 - (static_cast<double>(science_offset) / static_cast<double>(lock_offset))); // science_offset over lock_offset coef

    const std::size_t word_count_ = static_cast<std::size_t>(sol_coef * (hacking_max_words - hacking_min_words)) + hacking_min_words;

    return std::min<std::size_t>(20, word_count_);
}

template <class URBG>
    requires std::uniform_random_bit_generator<std::remove_reference_t<URBG>>
inline auto compute_word_length(yafth::lock_level lock_level_setting, URBG &&g) noexcept -> std::size_t
{
    return std::min<std::size_t>(12, 4 + 2 * static_cast<std::size_t>(lock_level_setting) + (std::forward<URBG>(g)() & 1));
}
} // namespace

namespace yafth::core
{

struct engine::impl
{
    impl(lock_level lock_level_setting, uint32_t player_science_skill, uint64_t seed) noexcept : rng{static_cast<uint32_t>(util::seed(seed))}
    {
        detail::symbol_manipulator::generate_term_chars(terminal, rng);

        const std::size_t word_length = compute_word_length(lock_level_setting, rng);
        const std::size_t word_count = compute_word_count(lock_level_setting, player_science_skill);

        const std::array<std::string, 20> word_list = detail::symbol_manipulator::generate_words(word_length, word_count, rng);
        const std::size_t answer_index = rng.next() % word_count;
        const std::array<uint16_t, 20> offsets = detail::symbol_manipulator::place_words(terminal, word_list, word_length, word_count, rng);

        words.init(word_length, word_count, answer_index, offsets);
    }

    auto check_coords(screen_coords coords) const noexcept -> std::optional<std::size_t>
    {
        auto m_x = coords.x;
        auto m_y = coords.y;

        if ((8 <= m_x && m_x <= 19) && (6 <= m_y && m_y <= 22)) // first window
        {
            m_x -= 8;
            m_y -= 6;
        }
        else if ((28 <= m_x && m_x <= 39) && (6 <= m_y && m_y <= 22)) // second window
        {
            m_x -= 28;
            m_y -= 6;
            m_y += 17;
        }
        else
        {
            return std::nullopt;
        }
        return 12 * m_y + m_x;
    }

    auto process_input(input current_input) noexcept -> state
    {
        std::optional<std::size_t> internal_coord;
        if (current_input.coords.has_value())
        {
            internal_coord = check_coords(*current_input.coords);
        }
        switch (current_input.type)
        {
        case input_type::other: // do nothing, return current state
            return {terminal.view(), state.attempts_left(), {}, {}};
            break;
        case input_type::look:
            if (internal_coord.has_value())
            {
                auto hl = detail::interaction_logic::look_at(*internal_coord, terminal, state);
                return {terminal.view(), state.attempts_left(), hl, std::nullopt};
            }
            return {terminal.view(), state.attempts_left(), std::nullopt, {}};
            break;
        case input_type::click:
            if (internal_coord.has_value())
            {
                auto click_res = detail::interaction_logic::click_at(*internal_coord, terminal, words, state, rng);
                return {terminal.view(), state.attempts_left(), std::nullopt, click_res};
            }
            return {terminal.view(), state.attempts_left(), std::nullopt, std::nullopt};
            break;
        }
        // should not happen
        return {terminal.view(), state.attempts_left(), std::nullopt, std::nullopt};
    }

    util::xorshift32 rng;
    detail::word_repository words;
    detail::terminal_buffer terminal;
    detail::game_state state;
};

engine::engine(const lock_level lock_level_setting, const uint32_t player_science_skill, const uint64_t seed = 0) noexcept
    : pimpl_(std::make_unique<impl>(lock_level_setting, player_science_skill, seed))
{
}

engine::~engine()
{
}

auto engine::process_input(input current_input) noexcept -> state
{
    return pimpl_->process_input(current_input);
}
} // namespace yafth::core