// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once
// stl
#include <array>
#include <bitset>
#include <cstddef>

namespace yafth::core::detail
{

class word_repository
{
  public:
    void init(std::size_t word_length, std::size_t word_count, std::size_t answer_index, std::array<uint16_t, 20> offsets) noexcept;

    [[nodiscard]] auto word_length() const noexcept -> std::size_t;
    [[nodiscard]] auto word_count() const noexcept -> std::size_t;
    [[nodiscard]] auto answer_index() const noexcept -> std::size_t;
    [[nodiscard]] auto words_left() const noexcept -> std::size_t;

    [[nodiscard]] auto is_word_offset(std::size_t offset) const noexcept -> bool;
    [[nodiscard]] auto is_answer_offset(std::size_t offset) const noexcept -> bool;

    void mark_removed(std::size_t offset) noexcept;
    [[nodiscard]] auto is_removed(std::size_t offset) const noexcept -> bool;

    [[nodiscard]] auto offsets() const noexcept -> const std::array<uint16_t, 20> &;

  private:
    std::array<uint16_t, 20> offsets_{};
    std::bitset<20> removed_{};

    uint8_t answer_index_{};
    uint8_t word_length_{};
    uint8_t word_count_{};
};

} // namespace yafth::core::detail
