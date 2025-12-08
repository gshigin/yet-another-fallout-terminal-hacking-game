#include "packed_word.hpp"
#include <gtest/gtest.h>

namespace {

using yafth::wordgen::packed_word;

class PackedWordTest : public ::testing::Test {
 protected:
  packed_word word_;
};

TEST_F(PackedWordTest, Empty) {
  // Arrange
  word_ = packed_word{""};

  // Act

  // Assert
  EXPECT_FALSE(word_.is_valid());
}

TEST_F(PackedWordTest, Default) {
  // Arrange

  // Act

  // Assert
  EXPECT_FALSE(word_.is_valid());
}

TEST_F(PackedWordTest, CopyConstructor) {
  // Arrange
  word_ = packed_word("abcdef");

  // Act
  auto word_cpy(word_);

  // Assert
  EXPECT_EQ(word_.size(), word_cpy.size());
  EXPECT_EQ(word_.is_valid(), word_cpy.is_valid());
  EXPECT_EQ(word_.raw(), word_cpy.raw());
  EXPECT_EQ(word_.get_word(), word_cpy.get_word());
}

TEST_F(PackedWordTest, CopyOperator) {
  // Arrange
  word_ = packed_word("abcdef");

  // Act
  packed_word word_cpy("lol");
  word_cpy = word_;

  // Assert
  EXPECT_EQ(word_.size(), word_cpy.size());
  EXPECT_EQ(word_.is_valid(), word_cpy.is_valid());
  EXPECT_EQ(word_.raw(), word_cpy.raw());
  EXPECT_EQ(word_.get_word(), word_cpy.get_word());
}

TEST_F(PackedWordTest, GetWord) {
  // Arrange
  const std::string string = "abcdef";
  word_ = packed_word{string};

  // Act
  const auto ref = word_.get_word();

  // Assert
  EXPECT_EQ(ref, string);
}

TEST_F(PackedWordTest, ExactWords) {
  // Arrange
  const std::string string = "abcdef";
  word_ = packed_word{string};

  // Act
  packed_word word2(string);
  const auto diff = word_.diff(word2);

  // Assert
  EXPECT_EQ(0, diff);
  EXPECT_EQ(word_, word2);
}

TEST_F(PackedWordTest, DifferentWords) {
  // Arrange
  const std::string string1 = "abcdef";
  const std::string string2 = "accdgf";

  word_ = packed_word{string1};

  // Act
  packed_word word2(string2);
  const auto diff = word_.diff(word2);

  // Assert
  EXPECT_EQ(2, diff);
}

TEST_F(PackedWordTest, TotalDifferentWords) {
  // Arrange
  const std::string string1 = "gaabcdef";
  const std::string string2 = "fedcbaag";

  word_ = packed_word{string1};

  // Act
  packed_word word2(string2);
  const auto diff = word_.diff(word2);

  // Assert
  EXPECT_EQ(word_.size(), diff);
}

TEST_F(PackedWordTest, OperatorBracket) {
  // Arrange
  const std::string string = "abcdef";
  word_ = packed_word{string};

  // Act

  // Assert
  EXPECT_EQ(word_[0], 'a');
  EXPECT_EQ(word_[1], 'b');
  EXPECT_EQ(word_[2], 'c');
  EXPECT_EQ(word_[3], 'd');
  EXPECT_EQ(word_[4], 'e');
  EXPECT_EQ(word_[5], 'f');
}

TEST_F(PackedWordTest, DiffWordsDifferentWords) {
  // Arrange
  const std::string string1 = "abcde";
  const std::string string2 = "abgde";

  word_ = packed_word{string1};
  packed_word word2(string2);

  // Act

  const auto dword = word_.diff_word(word2);

  // Assert
  EXPECT_EQ(word_.size(), dword.size());
  EXPECT_EQ(dword[0], packed_word::empty_symbol);
  EXPECT_EQ(dword[1], packed_word::empty_symbol);
  EXPECT_EQ(dword[2], 'g');
  EXPECT_EQ(dword[3], packed_word::empty_symbol);
  EXPECT_EQ(dword[4], packed_word::empty_symbol);
}

TEST_F(PackedWordTest, DiffWordsDifferentWords2) {
  // Arrange
  const std::string string1 = "abcdefghijkl";
  const std::string string2 = "abzdefghwjkl";

  word_ = packed_word{string1};
  packed_word word2(string2);

  // Act

  const auto dword = word_.diff_word(word2);

  // Assert
  EXPECT_EQ(word_.size(), dword.size());
  EXPECT_EQ(dword[0], packed_word::empty_symbol);
  EXPECT_EQ(dword[1], packed_word::empty_symbol);
  EXPECT_EQ(dword[2], 'z');
  EXPECT_EQ(dword[3], packed_word::empty_symbol);
  EXPECT_EQ(dword[4], packed_word::empty_symbol);
  EXPECT_EQ(dword[5], packed_word::empty_symbol);
  EXPECT_EQ(dword[6], packed_word::empty_symbol);
  EXPECT_EQ(dword[7], packed_word::empty_symbol);
  EXPECT_EQ(dword[8], 'w');
  EXPECT_EQ(dword[9], packed_word::empty_symbol);
  EXPECT_EQ(dword[10], packed_word::empty_symbol);
  EXPECT_EQ(dword[11], packed_word::empty_symbol);
}

TEST_F(PackedWordTest, Set) {
  // Arrange
  const std::string string = "abcde";

  word_ = packed_word{string};

  // Act
  word_.set(0, 'z');
  word_.set(3, 'w');

  // Assert
  EXPECT_EQ(word_.get_word(), "zbcwe");
}

struct PackedWordSizeParamCase {
  std::string str;
  bool expected_is_valid;
};

class PackedWordSizeParamTest : public ::testing::TestWithParam<PackedWordSizeParamCase> {
 protected:
  packed_word word_;
};

TEST_P(PackedWordSizeParamTest, IsValid) {
  // Arrange
  word_ = packed_word(GetParam().str);

  // Act
  bool valid = word_.is_valid();
  auto actual_size = word_.size();

  // Assert
  EXPECT_EQ(valid, GetParam().expected_is_valid);
}

INSTANTIATE_TEST_SUITE_P(PackedWordValidSizes,
                         PackedWordSizeParamTest,
                         ::testing::Values(PackedWordSizeParamCase{.str = "a", .expected_is_valid = false},
                                           PackedWordSizeParamCase{.str = "at", .expected_is_valid = false},
                                           PackedWordSizeParamCase{.str = "lol", .expected_is_valid = false},
                                           PackedWordSizeParamCase{.str = "name", .expected_is_valid = true},
                                           PackedWordSizeParamCase{.str = "clear", .expected_is_valid = true},
                                           PackedWordSizeParamCase{.str = "abobus", .expected_is_valid = true},
                                           PackedWordSizeParamCase{.str = "zostera", .expected_is_valid = true},
                                           PackedWordSizeParamCase{.str = "coloppes", .expected_is_valid = true},
                                           PackedWordSizeParamCase{.str = "grassplat", .expected_is_valid = true},
                                           PackedWordSizeParamCase{.str = "anderweigt", .expected_is_valid = true},
                                           PackedWordSizeParamCase{.str = "ilnonbestwi", .expected_is_valid = true},
                                           PackedWordSizeParamCase{.str = "howbtdiznuts", .expected_is_valid = true},
                                           PackedWordSizeParamCase{.str = "aaaaaaaaaaaaa", .expected_is_valid = false},
                                           PackedWordSizeParamCase{.str = "ababababababab", .expected_is_valid = false}));

}  // namespace