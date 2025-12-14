#include "bit_stream.hpp"
#include <gtest/gtest.h>

using namespace yafth::wordgen;

TEST(BitStream, WriteReadSingleBits) {
  // Arrange
  std::vector<uint8_t> buffer;
  BitWriter writer(buffer);

  // Act
  writer.write_bits(1, 1);
  writer.write_bits(0, 1);
  writer.write_bits(1, 1);
  writer.flush();

  BitReader reader(buffer);
  const auto a = reader.read_bits(1);
  const auto b = reader.read_bits(1);
  const auto c = reader.read_bits(1);

  // Assert
  EXPECT_EQ(a, 1);
  EXPECT_EQ(b, 0);
  EXPECT_EQ(c, 1);
}

TEST(BitStream, WriteRead64Bits) {
  // Arrange
  std::vector<uint8_t> buffer;
  BitWriter writer(buffer);

  constexpr uint64_t value = 0xFEDCBA9876543210ULL;

  // Act
  writer.write_bits(value, 64);
  writer.flush();

  BitReader reader(buffer);
  const auto decoded = reader.read_bits(64);

  // Assert
  EXPECT_EQ(decoded, value);
}

TEST(BitStream, CrossByteBoundaries) {
  // Arrange
  std::vector<uint8_t> buffer;
  BitWriter writer(buffer);

  // Act
  writer.write_bits(0b101, 3);
  writer.write_bits(0b11110000, 8);
  writer.write_bits(0b11, 2);
  writer.flush();

  BitReader reader(buffer);

  // Assert
  EXPECT_EQ(reader.read_bits(3), 0b101);
  EXPECT_EQ(reader.read_bits(8), 0b11110000);
  EXPECT_EQ(reader.read_bits(2), 0b11);
}

TEST(BitStream, MultipleWrites) {
  // Arrange
  std::vector<uint8_t> buffer;
  BitWriter writer(buffer);

  // Act
  for (int i = 0; i < 10; ++i) {
    writer.write_bits(i, 4);
  }
  writer.flush();

  BitReader reader(buffer);

  // Assert
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(reader.read_bits(4), static_cast<uint64_t>(i));
  }
}

TEST(BitStream, FlushPadsWithZeros) {
  // Arrange
  std::vector<uint8_t> buffer;
  BitWriter writer(buffer);

  // Act
  writer.write_bits(0b101, 3);
  writer.flush();

  BitReader reader(buffer);

  // Assert
  EXPECT_EQ(reader.read_bits(3), 0b101);
  EXPECT_EQ(reader.read_bits(5), 0b00000);
}

TEST(BitStream, ReadPastPhysicalEndThrows) {
  // Arrange
  std::vector<uint8_t> buffer;
  BitWriter writer(buffer);

  // Act
  writer.write_bits(0b1, 1);
  writer.flush();

  BitReader reader(buffer);

  // Assert
  EXPECT_NO_THROW(reader.read_bits(64));
  EXPECT_THROW(reader.read_bits(1), std::runtime_error);
}
