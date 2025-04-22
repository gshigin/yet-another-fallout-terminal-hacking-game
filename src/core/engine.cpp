// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
// yafth
#include <yafth/core/engine.h>
#include <yafth/core/engine_detail/symbol_manipulator.h>
#include <yafth/core/types.h>
#include <yafth/util/random.h>
// stl
#include <algorithm>
#include <cassert>
#include <numeric>
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

namespace yafth
{
class bad_engine_access : public std::exception
{
  public:
    [[nodiscard]] auto what() const noexcept -> const char * override
    {
        return "Trying to call yafth::engine when it's in Done state";
    }
};
} // namespace yafth

namespace yafth::core
{
engine::engine(const lock_level lock_level_setting, const uint32_t player_science_skill, const uint64_t seed = 0)
    : rng_{static_cast<uint32_t>(util::seed(seed))}
{
    engine_detail::symbol_manipulator::generate_term_chars(terminal_, rng_);

    const std::size_t word_length = compute_word_length(lock_level_setting, rng_);
    const std::size_t word_count = compute_word_count(lock_level_setting, player_science_skill);

    const std::array<std::string, 20> word_list = engine_detail::symbol_manipulator::generate_words(word_length, word_count, rng_);
    const std::size_t answer_index = rng_.next() % word_count;
    const std::array<uint16_t, 20> offsets = engine_detail::symbol_manipulator::place_words(terminal_, word_list, word_length, word_count, rng_);

    words_.init(word_length, word_count, answer_index, offsets);
}

auto engine::look_at(std::size_t i) const -> highlight
{
    assert(i < terminal_.size());

    if (::is_char(terminal_.get(i))) // case of word
    {
        auto l = i;
        while (l > 0 && ::is_char(terminal_.get(l - 1)))
        {
            --l;
        }

        auto r = i;
        while (r < terminal_.size() && ::is_char(terminal_.get(r)))
        {
            ++r;
        }

        return {l, r};
    }
    if (::is_open_br(terminal_.get(i)) && !state_.is_bracket_used(i)) // case of brackets
    {
        const size_t limit = ((i / 12) + 1) * 12;
        const char expected_close = ::bracket_lookup(terminal_.get(i));

        for (size_t index = i; index != limit; ++index)
        {
            char c = terminal_.get(index);

            if (::is_char(c))
            {
                break;
            }

            if (c == expected_close)
            {
                return {i, index + 1};
            }
        }
    }
    // case of one symbol
    return {i, i + 1};
}

// this code is shit
auto engine::click_at(std::size_t i) -> click_status
{
    assert(i < terminal_.size());

    if (state_.is_game_over())
    {
        throw yafth::bad_engine_access();
    }
    const auto &[b, e] = look_at(i);
    std::string_view substr = terminal_.slice(b, e);
    const std::string_view term = terminal_.view();
    if (substr.size() != 1)
    {
        if (words_.is_word_offset(b)) // word
        {
            if (words_.is_answer_offset(b))
            {
                state_.set_game_over();
                return {click_result::exact_match, {}};
            }
            else // it's not an answer
            {
                state_.consume_attempt();
                const std::size_t match = std::inner_product(substr.begin(), substr.end(), term.begin() + words_.offsets()[words_.answer_index()], 0,
                                                             std::plus<>(), std::equal_to<>());
                terminal_.replace_with_char(b, e);
                words_.mark_removed(b);

                if (state_.attempts_left() == 0)
                {
                    state_.set_game_over();
                    return {click_result::lockout_in_progress, word_match{match, words_.word_length()}};
                }

                return {click_result::entry_denied, word_match{match, words_.word_length()}};
            }
        }
        else // bracket
        {
            const std::size_t dist = std::distance<const char *>(term.data(),
                                                                 substr.data()); // seem like a problem, but works fine
            if (!state_.is_bracket_used(dist))
            {
                state_.mark_bracket_used(dist);

                if (words_.words_left() == 1)
                {
                    state_.reset_attempts();
                    return {click_result::allowance_replenished, {}};
                }
                else // words_left != 1
                {
                    if (state_.attempts_left() == 4)
                    {
                        // remove dud
                        std::size_t dud_number = rng_() % (words_.words_left() - 1);

                        for (auto offset : words_.offsets())
                        {
                            if (dud_number == 0)
                            {
                                dud_number = offset;
                                break;
                            }
                            if (!words_.is_removed(offset) && !words_.is_answer_offset(offset))
                            {
                                --dud_number;
                            }
                        }

                        terminal_.replace_with_char(dud_number, dud_number + words_.word_length());
                        words_.mark_removed(dud_number);

                        return {click_result::dud_removed, {}};
                    }
                    else // attemptsLeft != 4
                    {
                        if ((rng_() & 1) == 1)
                        {
                            // remove dud
                            std::size_t dud_number = rng_() % (words_.words_left() - 1);

                            for (auto offset : words_.offsets())
                            {
                                if (dud_number == 0)
                                {
                                    dud_number = offset;
                                    break;
                                }
                                if (!words_.is_removed(offset) && !words_.is_answer_offset(offset))
                                {
                                    --dud_number;
                                }
                            }

                            terminal_.replace_with_char(dud_number, dud_number + words_.word_length());
                            words_.mark_removed(dud_number);

                            return {click_result::dud_removed, {}};
                        }
                        else
                        {
                            state_.reset_attempts();
                            return {click_result::allowance_replenished, {}};
                        }
                    }
                }
            }
        }
    }
    return {click_result::error, {}};
}

// I'm lazy to change all constants to term_layout ones
auto engine::check_coords(screen_coords coords) const -> std::optional<std::size_t>
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

auto engine::process_input(input current_input) -> state
{
    std::optional<std::size_t> internal_coord;
    if (current_input.coords.has_value())
    {
        internal_coord = check_coords(*current_input.coords);
    }
    switch (current_input.type)
    {
    case input_type::other: // do nothing, return current state
        return {terminal_.view(), state_.attempts_left(), {}, {}};
        break;
    case input_type::look:
        if (internal_coord.has_value())
        {
            auto hl = look_at(*internal_coord);
            return {terminal_.view(), state_.attempts_left(), hl, std::nullopt};
        }
        return {terminal_.view(), state_.attempts_left(), std::nullopt, {}};
        break;
    case input_type::click:
        if (internal_coord.has_value())
        {
            auto click_res = click_at(*internal_coord);
            return {terminal_.view(), state_.attempts_left(), std::nullopt, click_res};
        }
        return {terminal_.view(), state_.attempts_left(), std::nullopt, std::nullopt};
        break;
    }
    // should not happen
    return {terminal_.view(), state_.attempts_left(), std::nullopt, std::nullopt};
}
} // namespace yafth::core