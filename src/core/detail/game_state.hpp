// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once
// stl
#include <bitset>

namespace yafth::core::detail
{

class game_state
{
    static constexpr uint16_t kSize = 408;

  public:
    [[nodiscard]] auto is_bracket_used(size_t index) const noexcept -> bool;
    void mark_bracket_used(size_t index) noexcept;

    [[nodiscard]] auto attempts_left() const noexcept -> size_t;
    auto consume_attempt() noexcept -> bool;
    void reset_attempts() noexcept;

    [[nodiscard]] auto is_game_over() const noexcept -> bool;
    void set_game_over() noexcept;

  private:
    std::bitset<kSize> used_brackets_{};
    uint8_t attempts_left_ = 4;
    bool game_over_ = false;
};

} // namespace yafth::core::detail
