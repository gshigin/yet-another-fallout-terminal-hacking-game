#include "packed_word.hpp"

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <ios>
#include <iostream>
#include <random>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {
using yafth::wordgen::packed_word;

constexpr int MAX_WORDS = 65536;

std::string_view getword(const char*& str, const char* end) {
  while (str < end && (*str == '\n' || *str == '\r'))
    ++str;

  const char* start = str;
  while (str < end && *str != '\n' && *str != '\r')
    ++str;

  return {start, str};
}

std::unordered_map<uint32_t, std::vector<packed_word>> load_words(const std::string& filename) {
  std::ifstream in(filename);
  std::vector<char> buf;

  in.seekg(0, std::ios::end);
  buf.resize(in.tellg());
  in.seekg(0, std::ios::beg);
  in.read(&buf[0], buf.size());

  const char* str = &buf[0];
  const char* strEnd = str + buf.size();

  std::string_view word;
  std::unordered_map<uint32_t, std::vector<packed_word>> ans;

  while (!(word = getword(str, strEnd)).empty()) {
    const auto size = word.size();
    if (size < 4 || size > 12)
      continue;
    ans[size].emplace_back(word);
  }

  return ans;
}

bool flexible_dfs(uint16_t current_idx,
                  const std::vector<packed_word>& words,
                  std::bitset<MAX_WORDS>& visited,
                  std::vector<uint16_t>& path,
                  int target_length,
                  int max_diff,
                  int& step_counter,
                  int step_limit = 300) {
  if ((int)path.size() >= target_length) {
    return true;
  }

  if (++step_counter > step_limit) {
    return false;
  }

  std::vector<std::pair<int, uint16_t>> candidates;

  const packed_word current_word = words[current_idx];

  for (uint16_t idx = 0; idx < static_cast<uint16_t>(words.size()); ++idx) {
    if (visited[idx]) {
      continue;
    }

    int diff = current_word.diff(words[idx]);
    if (diff > 0 && diff <= max_diff) {
      candidates.emplace_back(diff, idx);
    }
  }

  // std::sort(candidates.begin(), candidates.end());

  for (const auto& [diff, idx] : candidates) {
    visited[idx] = true;
    path.push_back(idx);
    if (flexible_dfs(idx, words, visited, path, target_length, max_diff, step_counter)) {
      return true;
    }
    path.pop_back();
    visited[idx] = false;
  }

  return false;
}

// ------------------------- BitWriter -------------------------
class BitWriter {
  std::vector<uint8_t>& out;
  uint8_t buffer = 0;
  int bits_filled = 0;

 public:
  BitWriter(std::vector<uint8_t>& output) : out(output) {}

  void write_bits(uint32_t value, int count) {
    while (count > 0) {
      int to_write = std::min(8 - bits_filled, count);
      buffer <<= to_write;
      buffer |= (value >> (count - to_write)) & ((1 << to_write) - 1);
      bits_filled += to_write;
      count -= to_write;

      if (bits_filled == 8) {
        out.push_back(buffer);
        buffer = 0;
        bits_filled = 0;
      }
    }
  }

  void flush() {
    if (bits_filled > 0) {
      buffer <<= (8 - bits_filled);
      out.push_back(buffer);
      buffer = 0;
      bits_filled = 0;
    }
  }
};

// ------------------------- BitReader -------------------------
class BitReader {
  const std::vector<uint8_t>& in;
  size_t byte_index = 0;
  int bits_left = 0;
  uint8_t buffer = 0;

 public:
  BitReader(const std::vector<uint8_t>& input) : in(input) {}

  uint32_t read_bits(int count) {
    uint32_t result = 0;
    while (count > 0) {
      if (bits_left == 0) {
        if (byte_index >= in.size())
          throw std::runtime_error("Out of input");
        buffer = in[byte_index++];
        bits_left = 8;
      }

      int take = std::min(bits_left, count);
      result <<= take;
      result |= (buffer >> (bits_left - take)) & ((1 << take) - 1);
      bits_left -= take;
      count -= take;
    }
    return result;
  }
};

// ------------------------- Helpers -------------------------
int get_position_bits(size_t word_len) {
  if (word_len <= 4) {
    return 2;
  }
  if (word_len <= 8) {
    return 3;
  }
  return 4;
}

// ------------------------- Encoding -------------------------
std::vector<uint8_t> encode_chain_compact_packed(const std::vector<uint16_t>& chain, const std::vector<packed_word>& words) {
  if (chain.empty()) {
    throw std::invalid_argument("Empty chain");
  }

  packed_word packed = words[chain[0]];
  const int word_len = words[chain[0]].size();
  const int pos_bits = get_position_bits(word_len);

  std::vector<uint8_t> buffer(8);
  std::memcpy(buffer.data(), &packed, sizeof(packed));

  buffer.push_back(static_cast<uint8_t>(chain.size() - 1));

  BitWriter writer(buffer);
  for (size_t i = 1; i < chain.size(); ++i) {
    packed_word prev = words[chain[i - 1]];
    packed_word curr = words[chain[i]];

    if (prev.size() != curr.size()) {
      throw std::invalid_argument("Word lengths mismatch");
    }

    auto diff_word = prev.diff_word(curr);
    auto diff_size = prev.diff(curr);

    if (diff_size == 0 || diff_size > 3) {
      throw std::runtime_error("Invalid number of diffs");
    }

    uint8_t diff_code = diff_size == 1 ? 0b01 : diff_size == 2 ? 0b10 : 0b11;
    writer.write_bits(diff_code, 2);

    for (uint8_t pos = 0; pos < diff_word.size(); ++pos) {
      const auto c = diff_word[pos];
      if (c != packed_word::empty_symbol) {
        writer.write_bits(pos, pos_bits);
        writer.write_bits(c - 'a' + 5, 5);
      }
    }
  }

  writer.flush();
  return buffer;
}

size_t chain_compact_packed_size(const std::vector<uint16_t>& chain, const std::vector<packed_word>& words) {
  if (chain.empty()) {
    throw std::invalid_argument("Empty chain");
  }

  packed_word packed = words[chain[0]];
  const int word_len = words[chain[0]].size();
  const int pos_bits = get_position_bits(word_len);

  size_t size = 8 * (sizeof(packed_word) + sizeof(uint8_t));

  for (size_t i = 1; i < chain.size(); ++i) {
    packed_word prev = words[chain[i - 1]];
    packed_word curr = words[chain[i]];

    if (prev.size() != curr.size()) {
      throw std::invalid_argument("Word lengths mismatch");
    }

    auto diff_size = prev.diff(curr);

    if (diff_size == 0 || diff_size > 3) {
      throw std::runtime_error("Invalid number of diffs");
    }

    uint8_t diff_code = diff_size == 1 ? 0b01 : diff_size == 2 ? 0b10 : 0b11;
    size += 2;

    size += (pos_bits + 5) * diff_size;
  }

  return (size + 7) / 8;
  /*
    64 (init word) + 8 (word num) + [ 2 (diff code) + < 2/3/4 (word position) + 5 (different char) > * 1/2/3 (difference size) ]
  */
}

// ------------------------- Decoding -------------------------
std::vector<packed_word> decode_chain_compact_packed(const std::vector<uint8_t>& data) {
  if (data.size() < 9) {
    throw std::runtime_error("Too short");
  }

  packed_word packed;
  std::memcpy(&packed, data.data(), sizeof(packed));

  size_t word_len = packed.size();
  const int pos_bits = get_position_bits(word_len);

  BitReader reader(data);
  reader.read_bits(64);  // skip first 64 bits (word)
  size_t steps = reader.read_bits(8);

  std::vector<packed_word> chain = {packed};
  for (int i = 0; i < steps; ++i) {
    packed_word current = chain.back();
    size_t code = reader.read_bits(2);
    int count = code == 0b01 ? 1 : code == 0b10 ? 2 : 3;

    for (int j = 0; j < count; ++j) {
      size_t pos = reader.read_bits(pos_bits);
      size_t letter = reader.read_bits(5);

      current.set(pos, static_cast<char>('a' + letter - 5));
    }

    chain.push_back(current);
  }

  return chain;
}

std::string generate_header_file(std::unordered_map<uint32_t, std::vector<packed_word>>& all_words, uint32_t seed, const std::string& header_output_path) {
  std::mt19937 rng(seed);
  std::ofstream header_file(header_output_path);

  if (!header_file.is_open()) {
    std::cerr << "Failed to open header output file\n";
    return "Error opening header file";
  }

  header_file << "#pragma once\n\n";

  header_file << "#include <array>\n";
  header_file << "#include <cstdint>\n\n";

  header_file << "namespace yafth::core::engine_detail::words {\n\n";

  std::bitset<65536> visited;
  std::vector<uint16_t> path;
  std::vector<uint8_t> compact_path;
  size_t compact_path_size;

  for (int word_length = 4; word_length <= 12; ++word_length) {
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "Chain of length " << word_length << " generation started\n";

    auto& words = all_words[word_length];  // Assuming that words are loaded by length
    std::shuffle(words.begin(), words.end(), rng);

    for (int target_length = 100; target_length != 0; --target_length) {
      size_t fail_counter = 0;

      for (uint16_t start_idx = 0; start_idx < static_cast<uint16_t>(words.size()); ++start_idx) {
        visited.reset();
        path.clear();
        compact_path.clear();

        visited[start_idx] = true;
        path.push_back(start_idx);

        int step_counter = 0;
        if (flexible_dfs(start_idx, words, visited, path, target_length, 3, step_counter)) {
          // compact_path = encode_chain_compact_packed(path, words);
          compact_path_size = chain_compact_packed_size(path, words);

          if (compact_path_size == 128)
            break;

          if (compact_path_size > 140)
            break;

          ++fail_counter;
          if (++fail_counter > 100)
            break;
        }
      }
      if (compact_path_size == 128) {
        compact_path = encode_chain_compact_packed(path, words);

        auto gen_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = gen_time - start;
        std::cout << "Chain of length " << word_length << " generated in " << diff.count() << " seconds\n";
        auto decoded = decode_chain_compact_packed(compact_path);

        std::vector<packed_word> ref;
        ref.reserve(path.size());
        for (auto id : path) {
          ref.emplace_back(words[id]);
        }

        if (ref != decoded) {
          throw std::runtime_error("Chain validation error!");
        } else {
          auto val_time = std::chrono::high_resolution_clock::now();
          std::chrono::duration<double> diff = val_time - gen_time;
          std::cout << "Chain of length " << word_length << " validated in " << diff.count() << " seconds\n";
        }

        std::cout << "Chain of length " << word_length << " : ";
        for (auto w : decoded) {
          std::cout << w.get_word() << ' ';
        }
        std::cout << '\n';

        header_file << "constexpr static std::array<uint8_t, 128> packed_word_" << std::dec << word_length << std::hex << " = { ";

        for (size_t i = 0; i < compact_path.size(); ++i) {
          header_file << "0x" << std::setw(2) << std::setfill('0') << std::hex << (int)compact_path[i];
          if (i < compact_path.size() - 1)
            header_file << ", ";
        }

        header_file << " };\n";
        break;
      }
    }
  }

  header_file << "\n} // namespace yafth::core::engine_detail::words\n";
  header_file.close();
  return "Header file generated successfully.";
}

}  // namespace

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <word dictionary path> <header output path> <random seed>" << std::endl;
    return 1;
  }

  std::string dictionary_path = argv[1];
  std::string header_output_path = argv[2];
  uint32_t seed = std::stoi(argv[3]);

  // Load words from dictionary (use the `load_words` function from your original code)
  auto start = std::chrono::high_resolution_clock::now();

  auto all_words = load_words(dictionary_path);

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = end - start;

  std::cerr << "load_words took " << diff.count() << " seconds\n";

  // Generate header file
  std::string result = generate_header_file(all_words, seed, header_output_path);

  std::cout << result << '\n';
  return 0;
}