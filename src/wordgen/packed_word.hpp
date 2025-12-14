#pragma once

#include <bit>
#include <cstdint>
#include <string>

namespace yafth::wordgen {

class packed_word {
 public:
  static constexpr char empty_symbol = '\\';
  packed_word() = default;
  explicit packed_word(const std::string_view word_view) noexcept : data_{pack_word(word_view)} {}

  [[nodiscard]] uint8_t size() const noexcept { return (data_ >> 60) & 0xF; }
  [[nodiscard]] bool is_valid() const noexcept { return data_ != 0; }
  [[nodiscard]] uint64_t raw() const noexcept { return data_; }
  [[nodiscard]] std::string get_word() const noexcept {
    if (!is_valid()) [[unlikely]] {
      return "";
    }

    std::string word(size(), 'a');

    size_t shift = 60 - 5;
    for (char& c : word) {
      const uint8_t v = (data_ >> shift) & 0x1F;
      c = static_cast<char>(c + v - 5);
      shift -= 5;
    }
    return word;
  }

  [[nodiscard]] uint8_t diff(packed_word other) const noexcept {
    if (!is_valid() || !other.is_valid()) [[unlikely]] {
      return std::numeric_limits<uint8_t>::max();
    }

    if (size() != other.size()) [[unlikely]] {
      return std::numeric_limits<uint8_t>::max();
    }

    auto x = data_ ^ other.data_;
    // accumulate every 5-bit pack into lower bit
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 1;
    // mask lower bit in pack
    x &= 0x084210842108421ULL;

    return std::popcount(x);
  }

  [[nodiscard]] packed_word diff_word(packed_word other) const noexcept {
    if (!is_valid() || !other.is_valid()) {
      return {};
    }

    if (size() != other.size()) [[unlikely]] {
      return {};
    }

    auto x = data_ ^ other.data_;
    // accumulate every 5-bit pack into lower bit
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 1;
    // mask lower bit in pack
    x &= 0x084210842108421ULL;
    // expand back to whole pack
    x *= 0x1FULL;

    other.data_ &= x;
    other.data_ |= data_ & (0b1111ULL << 60);

    return other;
  }

  [[nodiscard]] char operator[](size_t idx) const noexcept {
    // idx %= size();

    const uint8_t v = (data_ >> ((11 - idx) * 5)) & 0x1F;
    return static_cast<char>('a' + v - 5);
  }

  void set(size_t idx, char c) noexcept {
    // idx %= size();

    const size_t bitpos = (11 - idx) * 5;

    data_ &= ~(uint64_t(0x1F) << bitpos);
    data_ |= (uint64_t(c - 'a' + 5) << bitpos);
  }

  bool operator==(packed_word other) const noexcept { return data_ == other.data_; }

 private:
  uint64_t data_{0};

  static uint64_t pack_word(std::string_view word) noexcept {
    if (word.size() < 4 || word.size() > 12) [[unlikely]] {
      return 0;
    }

    uint64_t result = word.size() & 0xF;

    for (const char c : word) {
      result <<= 5;
      result |= (c - 'a' + 5) & 0x1F;
    }

    size_t padding = (12 - word.size()) * 5;
    result <<= padding;
    return result;
  }
};
}  // namespace yafth::wordgen