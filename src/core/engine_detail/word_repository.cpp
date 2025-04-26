// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
// yafth
#include <yafth/core/engine_detail/word_repository.h>
// stl
#include <algorithm>
#include <cassert>

namespace yafth::core::engine_detail
{

void word_repository::init(std::size_t word_length, std::size_t word_count, std::size_t answer_index, std::array<uint16_t, 20> offsets)
{
    assert(4 <= word_length && word_length <= 12);
    assert(5 <= word_count && word_count <= 20);
    assert(answer_index < word_count);

    word_length_ = static_cast<uint8_t>(word_length);
    word_count_ = static_cast<uint8_t>(word_count);
    answer_index_ = static_cast<uint8_t>(answer_index);
    offsets_ = offsets;

    for (int i = word_count_; i < offsets_.size(); ++i)
    {
        removed_.set(i);
    }
}

auto word_repository::word_length() const noexcept -> std::size_t
{
    return word_length_;
}

auto word_repository::word_count() const noexcept -> std::size_t
{
    return word_count_;
}

auto word_repository::answer_index() const noexcept -> std::size_t
{
    return answer_index_;
}

auto word_repository::words_left() const noexcept -> std::size_t
{
    return 20 - removed_.count();
}

auto word_repository::offsets() const noexcept -> const std::array<uint16_t, 20> &
{
    return offsets_;
}

auto word_repository::is_word_offset(std::size_t offset) const noexcept -> bool
{
    return std::find(offsets_.begin(), offsets_.end(), offset) != offsets_.end();
}

auto word_repository::is_answer_offset(std::size_t offset) const noexcept -> bool
{
    return offsets_[answer_index_] == offset;
}

void word_repository::mark_removed(std::size_t offset) noexcept
{
    auto it = std::find(offsets_.begin(), offsets_.end(), offset);
    if (it != offsets_.end())
    {
        removed_.set(std::distance(offsets_.begin(), it));
    }
}

auto word_repository::is_removed(std::size_t offset) const noexcept -> bool
{
    auto it = std::find(offsets_.begin(), offsets_.end(), offset);
    if (it != offsets_.end())
    {
        return removed_.test(std::distance(offsets_.begin(), it));
    }
    return false;
}
} // namespace yafth::core::engine_detail