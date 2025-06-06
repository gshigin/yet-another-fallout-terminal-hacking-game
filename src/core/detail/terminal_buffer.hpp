// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once
// stl
#include <array>
#include <string_view>

namespace yafth::core::detail
{
class terminal_buffer
{
    static constexpr size_t kSize = 408;

  public:
    [[nodiscard]] constexpr auto size() const noexcept -> size_t
    {
        return kSize;
    }

    [[nodiscard]] auto get(size_t index) const noexcept -> char;
    void set(size_t index, char value) noexcept;

    [[nodiscard]] auto slice(size_t start, size_t end) const noexcept -> std::string_view;
    [[nodiscard]] auto view() const noexcept -> std::string_view;

    void replace_with_char(size_t start, size_t end, char c = '.') noexcept;
    void replace_with_string(size_t start, std::string_view str) noexcept;

    [[nodiscard]] auto raw() const noexcept -> const std::array<char, kSize> &;

  private:
    std::array<char, kSize> chars_{};
};
} // namespace yafth::core::detail