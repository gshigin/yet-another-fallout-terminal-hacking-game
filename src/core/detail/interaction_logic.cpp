// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
// yafth
#include "interaction_logic.hpp"
#include "game_state.hpp"
#include "terminal_buffer.hpp"
#include "word_repository.hpp"
// stl
#include <cassert>
#include <numeric>

namespace
{
constexpr auto bracket_lookup(char c) noexcept -> char
{
    constexpr std::array<std::pair<char, char>, 4> brackets{{{'{', '}'}, {'(', ')'}, {'<', '>'}, {'[', ']'}}};

    return std::find_if(begin(brackets), end(brackets), [&c](const auto &v) { return v.first == c; })->second;
}

constexpr auto is_open_bracket(char c) noexcept -> bool
{
    return c == '(' || c == '[' || c == '<' || c == '{';
}

constexpr auto is_char(char c) noexcept -> bool
{
    return c >= 'A' && c <= 'Z';
}
} // namespace

auto yafth::core::detail::interaction_logic::look_at(std::size_t index, const terminal_buffer &terminal, game_state &state) noexcept -> highlight
{
    assert(index < terminal.size());

    if (::is_char(terminal.get(index))) // case of word
    {
        auto l = index;
        while (l > 0 && is_char(terminal.get(l - 1)))
        {
            --l;
        }

        auto r = index;
        while (r < terminal.size() && ::is_char(terminal.get(r)))
        {
            ++r;
        }

        return {l, r};
    }
    if (is_open_bracket(terminal.get(index)) && !state.is_bracket_used(index)) // case of brackets
    {
        const size_t limit = ((index / 12) + 1) * 12;
        const char expected_close = bracket_lookup(terminal.get(index));

        for (size_t end_index = index; end_index != limit; ++end_index)
        {
            char c = terminal.get(end_index);

            if (::is_char(c))
            {
                break;
            }

            if (c == expected_close)
            {
                return {index, end_index + 1};
            }
        }
    }
    // case of one symbol
    return {index, index + 1};
}

auto yafth::core::detail::interaction_logic::_internal::click_at(std::size_t index, terminal_buffer &terminal, word_repository &words, game_state &state,
                                                                 rng_fref gen_next) noexcept -> click_status
{
    assert(index < terminal.size());
    assert(!state.is_game_over());
    const auto &[b, e] = look_at(index, terminal, state);
    std::string_view substr = terminal.slice(b, e);
    const std::string_view term = terminal.view();
    if (substr.size() != 1)
    {
        if (words.is_word_offset(b)) // word
        {
            if (words.is_answer_offset(b))
            {
                state.set_game_over();
                return {click_result::exact_match, {}};
            }
            else // it's not an answer
            {
                state.consume_attempt();
                const std::size_t match =
                    std::inner_product(substr.begin(), substr.end(), term.begin() + words.offsets()[words.answer_index()], 0, std::plus<>(), std::equal_to<>());
                terminal.replace_with_char(b, e);
                words.mark_removed(b);

                if (state.attempts_left() == 0)
                {
                    state.set_game_over();
                    return {click_result::lockout_in_progress, word_match{match, words.word_length()}};
                }

                return {click_result::entry_denied, word_match{match, words.word_length()}};
            }
        }
        else // bracket
        {
            const std::size_t dist = std::distance<const char *>(term.data(),
                                                                 substr.data()); // seem like a problem, but works fine
            if (!state.is_bracket_used(dist))
            {
                state.mark_bracket_used(dist);

                if (words.words_left() == 1)
                {
                    state.reset_attempts();
                    return {click_result::allowance_replenished, {}};
                }
                else // wordsleft != 1
                {
                    if (state.attempts_left() == 4)
                    {
                        // remove dud
                        std::size_t dud_number = gen_next() % (words.words_left() - 1);

                        for (auto offset : words.offsets())
                        {
                            if (dud_number == 0)
                            {
                                dud_number = offset;
                                break;
                            }
                            if (!words.is_removed(offset) && !words.is_answer_offset(offset))
                            {
                                --dud_number;
                            }
                        }

                        terminal.replace_with_char(dud_number, dud_number + words.word_length());
                        words.mark_removed(dud_number);

                        return {click_result::dud_removed, {}};
                    }
                    else // attemptsLeft != 4
                    {
                        if ((gen_next() & 1) == 1)
                        {
                            // remove dud
                            std::size_t dud_number = gen_next() % (words.words_left() - 1);

                            for (auto offset : words.offsets())
                            {
                                if (dud_number == 0)
                                {
                                    dud_number = offset;
                                    break;
                                }
                                if (!words.is_removed(offset) && !words.is_answer_offset(offset))
                                {
                                    --dud_number;
                                }
                            }

                            terminal.replace_with_char(dud_number, dud_number + words.word_length());
                            words.mark_removed(dud_number);

                            return {click_result::dud_removed, {}};
                        }
                        else
                        {
                            state.reset_attempts();
                            return {click_result::allowance_replenished, {}};
                        }
                    }
                }
            }
        }
    }
    return {click_result::error, {}};
}