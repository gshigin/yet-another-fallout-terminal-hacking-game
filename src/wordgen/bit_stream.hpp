#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace yafth::wordgen {

class BitWriter {
 public:
  explicit BitWriter(std::vector<uint8_t>& output) : out_(output) {}

  void write_bits(uint64_t value, int count) {
    assert(count > 0 && count <= 64);

    while (count > 0) {
      const int free_bits = 64 - bits_filled_;
      const int to_write = std::min(free_bits, count);

      buffer_ <<= to_write;

      const uint64_t mask = (to_write == 64) ? ~uint64_t(0) : ((uint64_t(1) << to_write) - 1);

      buffer_ |= (value >> (count - to_write)) & mask;

      bits_filled_ += to_write;
      count -= to_write;

      if (bits_filled_ == 64) {
        flush_block();
      }
    }
  }

  void flush() {
    if (bits_filled_ == 0) {
      return;
    }

    buffer_ <<= (64 - bits_filled_);
    bits_filled_ = 64;
    flush_block();
  }

 private:
  void flush_block() {
    for (int i = 7; i >= 0; --i) {
      out_.push_back(static_cast<uint8_t>((buffer_ >> (i * 8)) & 0xFF));
    }
    buffer_ = 0;
    bits_filled_ = 0;
  }

  std::vector<uint8_t>& out_;
  uint64_t buffer_ = 0;
  int bits_filled_ = 0;
};

class BitReader {
 public:
  explicit BitReader(const std::vector<uint8_t>& input) : in_(input) {}

  uint64_t read_bits(int count) {
    assert(count > 0 && count <= 64);

    uint64_t result = 0;

    while (count > 0) {
      while (bits_left_ < count && byte_index_ < in_.size()) {
        buffer_ = (buffer_ << 8) | in_[byte_index_++];
        bits_left_ += 8;
      }

      if (bits_left_ < count) {
        throw std::runtime_error("BitReader is out of input");
      }

      const int take = std::min(bits_left_, count);
      const uint64_t mask = (take == 64) ? ~uint64_t(0) : ((uint64_t(1) << take) - 1);

      result <<= take;
      result |= (buffer_ >> (bits_left_ - take)) & mask;

      bits_left_ -= take;
      count -= take;
    }

    return result;
  }

 private:
  const std::vector<uint8_t>& in_;
  size_t byte_index_ = 0;
  uint64_t buffer_ = 0;
  int bits_left_ = 0;
};

}  // namespace yafth::wordgen
