// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once
// yafth
#include <yafth/core/engine_detail/game_state.h>
#include <yafth/core/engine_detail/terminal_buffer.h>
#include <yafth/core/terminal_layout.h>
#include <yafth/core/types.h>
#include <yafth/util/random.h>
// stl
#include <array>
#include <cstdint>
#include <optional>

namespace
{
constexpr size_t visible_chars_num =
    terminal_layout::interaction_window::interactive_window_1::height * terminal_layout::interaction_window::interactive_window_1::width +
    terminal_layout::interaction_window::interactive_window_2::height * terminal_layout::interaction_window::interactive_window_2::width;
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
    engine() : engine(lock_level::average, 50, 0){};

    auto process_input(input input) -> state;

  private:
    [[nodiscard]] auto check_coords(screen_coords coords) const -> std::optional<std::size_t>;
    [[nodiscard]] auto look_at(std::size_t i) const -> highlight;
    auto click_at(std::size_t i) -> click_status;

    auto set_word_count() noexcept -> std::size_t;
    auto set_word_length() noexcept -> std::size_t;
    void generate_term_chars() noexcept;
    void generate_words() noexcept;

    util::xoroshiro128 rng;

    lock_level lock_level_setting_;
    uint32_t player_science_skill_;

    std::size_t word_length_;
    std::size_t word_count_;
    std::size_t answer_;
    std::size_t words_left_;

    std::array<std::size_t, 20> words_;

    engine_detail::terminal_buffer terminal_;
    engine_detail::game_state state_;
};
} // namespace yafth::core