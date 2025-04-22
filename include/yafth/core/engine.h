// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once
// yafth
#include "yafth/core/engine_detail/word_repository.h"
#include <yafth/core/engine_detail/game_state.h>
#include <yafth/core/engine_detail/terminal_buffer.h>
#include <yafth/core/terminal_layout.h>
#include <yafth/core/types.h>
#include <yafth/util/random.h>
// stl
#include <cstdint>
#include <optional>

namespace yafth::core
{
class engine
{
  public:
    engine(lock_level lock_level_setting, uint32_t player_science_skill, uint64_t seed);
    engine() : engine(lock_level::average, 50, 0){};

    auto process_input(input input) -> state;

  private:
    [[nodiscard]] auto check_coords(screen_coords coords) const -> std::optional<std::size_t>;
    [[nodiscard]] auto look_at(std::size_t i) const -> highlight;
    auto click_at(std::size_t i) -> click_status;

    util::xorshift32 rng_;
    engine_detail::word_repository words_;
    engine_detail::terminal_buffer terminal_;
    engine_detail::game_state state_;
};
} // namespace yafth::core