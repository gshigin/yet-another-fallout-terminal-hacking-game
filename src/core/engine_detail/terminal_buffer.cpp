#include <yafth/core/engine_detail/terminal_buffer.h>

#include <algorithm>
#include <cassert>

namespace yafth::core::engine_detail
{
auto terminal_buffer::get(size_t index) const noexcept -> char
{
    assert(index < chars_.size());

    return chars_[index];
}

void terminal_buffer::set(size_t index, char value) noexcept
{
    assert(index < chars_.size());

    chars_[index] = value;
}

auto terminal_buffer::slice(size_t start, size_t end) const noexcept -> std::string_view
{
    assert(start <= end);
    assert(end < chars_.size());

    return {chars_.begin() + start, chars_.begin() + end};
}

auto terminal_buffer::view() const noexcept -> std::string_view
{
    return {chars_.begin(), chars_.end()};
}

void terminal_buffer::replace_with_char(size_t start, size_t end, char c) noexcept
{
    assert(start <= end);
    assert(end < chars_.size());

    std::fill(chars_.begin() + start, chars_.begin() + end, c);
}

void terminal_buffer::replace_with_string(size_t start, std::string_view str) noexcept
{
    assert(start + str.size() < chars_.size());

    std::copy(str.begin(), str.end(), chars_.begin() + start);
}

auto terminal_buffer::raw() const noexcept -> const std::array<char, kSize> &
{
    return chars_;
}
} // namespace yafth::core::engine_detail