// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once
// yafth
#include <yafth/core/terminal_layout.h>
#include <yafth/core/types.h>
#include <yafth/util/random.h>
// stl
#include <array>
#include <bitset>
#include <cstdint>
#include <optional>

namespace
{
constexpr size_t visible_chars_num = terminal_layout::interaction_window::interactive_window_1::height *
                                         terminal_layout::interaction_window::interactive_window_1::width +
                                     terminal_layout::interaction_window::interactive_window_2::height *
                                         terminal_layout::interaction_window::interactive_window_2::width;
}

namespace yafth::core
{
class engine
{
    enum class engine_state
    {
        running,
        done
    };

  public:
    engine(lock_level lock_level_setting, uint32_t player_science_skill, uint64_t seed);
    engine() : engine(lock_level::average, 50, 0) {};

    state process_input(input input);

  private:
    std::optional<std::size_t> check_coords(screen_coords coords) const;
    highlight look_at(std::size_t i) const;
    click_status click_at(std::size_t i);

    std::size_t set_word_count() noexcept;
    std::size_t set_word_length() noexcept;
    void generate_term_chars() noexcept;
    void generate_words() noexcept;

    util::xoroshiro128 rng;

    const lock_level lock_level_setting_;
    const uint32_t player_science_skill_;
    const std::size_t word_length_;
    const std::size_t word_count_;
    const std::size_t answer_;

    std::size_t attempts_left_ = 4;
    std::size_t words_left_;

    std::array<std::size_t, 20> words_;
    std::array<char, visible_chars_num> term_chars_;

    std::bitset<::visible_chars_num> used_bracket_;

    engine_state internal_state_ = engine_state::running;
};
} // namespace yafth::core