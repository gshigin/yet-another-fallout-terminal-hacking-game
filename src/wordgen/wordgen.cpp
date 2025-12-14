#include "bit_stream.hpp"
#include "packed_word.hpp"

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <ios>
#include <iostream>
#include <limits>
#include <random>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {
using yafth::wordgen::BitReader;
using yafth::wordgen::BitWriter;
using yafth::wordgen::packed_word;

static constexpr size_t max_words_count = std::numeric_limits<uint16_t>::max();
static constexpr size_t word_id_nan = std::numeric_limits<uint16_t>::max();

static constexpr size_t max_word_neighbours = 8;
static constexpr size_t max_chain_length = 50;

static constexpr std::array<uint8_t, 13> posbitarr = {255, 255, 255, 255, 2, 3, 3, 3, 3, 4, 4, 4, 4};
static constexpr std::array<uint8_t, 4> diffarr = {0b00, 0b01, 0b10, 0b11};

inline constexpr size_t word_encoding_bits(size_t word_len, size_t diff_size) {
  return 2 + (posbitarr[word_len] + 5) * diff_size;
}

inline std::string_view getword(const char*& str, const char* end) {
  while (str < end && (*str == '\n' || *str == '\r')) {
    ++str;
  }

  const char* start = str;
  while (str < end && *str != '\n' && *str != '\r') {
    ++str;
  }

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
    if (size < 4 || size > 12) {
      continue;
    }
    ans[size].emplace_back(word);
  }

  return ans;
}

std::vector<uint8_t> encode_chain_compact_packed(const std::vector<uint16_t>& chain, const std::vector<packed_word>& words) {
  if (chain.empty()) {
    throw std::invalid_argument("Trying to encode empty chain");
  }

  packed_word packed = words[chain[0]];
  const int word_len = words[chain[0]].size();
  const int pos_bits = posbitarr[word_len];

  std::vector<uint8_t> buffer;
  BitWriter writer(buffer);

  writer.write_bits(packed.raw(), 64);
  writer.write_bits(static_cast<uint8_t>(chain.size() - 1), 8);

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

    uint8_t diff_code = diffarr[diff_size];
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

std::vector<packed_word> decode_chain_compact_packed(const std::vector<uint8_t>& data) {
  if (data.size() < 9) {
    throw std::runtime_error("Decoding input is too short");
  }

  BitReader reader(data);
  uint64_t raw_word = reader.read_bits(64);
  packed_word packed = *reinterpret_cast<packed_word*>(&raw_word);

  const size_t word_len = packed.size();
  const int pos_bits = posbitarr[word_len];

  size_t steps = reader.read_bits(8);

  std::vector<packed_word> chain = {packed};
  for (int i = 0; i < steps; ++i) {
    packed_word current = chain.back();
    size_t code = reader.read_bits(2);

    if (code == 0) {
      throw std::runtime_error("Invalid diff code 0");
    }

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

struct chain_search_context {
  std::array<uint16_t, max_chain_length * max_word_neighbours> neighbours{};
  std::array<uint8_t, max_chain_length> neighbour_idx{};
  std::bitset<max_words_count> visited{};

  std::vector<uint16_t> chain;
  size_t chain_size_bits = 0;
};

void fill_neighbours(chain_search_context& ctx, const std::vector<packed_word>& words, size_t depth, size_t max_diff) {
  const packed_word current = words[ctx.chain.back()];

  auto neighs = std::span<uint16_t, max_word_neighbours>(ctx.neighbours.begin() + depth * max_word_neighbours, max_word_neighbours);
  std::fill(neighs.begin(), neighs.end(), word_id_nan);

  for (uint16_t idx = 0, n = 0; idx < static_cast<uint16_t>(words.size()) && n < max_word_neighbours; ++idx) {
    if (ctx.visited[idx]) {
      continue;
    }

    const int diff = current.diff(words[idx]);
    if (diff > 0 && diff <= static_cast<int>(max_diff)) {
      neighs[n++] = idx;
    }
  }

  ctx.neighbour_idx[depth] = 0;
}

inline void push_word(chain_search_context& ctx, uint16_t word_id, size_t word_len, size_t diff) {
  ctx.visited.set(word_id);
  ctx.chain.push_back(word_id);
  ctx.chain_size_bits += word_encoding_bits(word_len, diff);
}

inline void pop_word(chain_search_context& ctx, size_t word_len, size_t diff) {
  const uint16_t id = ctx.chain.back();
  ctx.chain.pop_back();
  ctx.visited.reset(id);
  ctx.chain_size_bits -= word_encoding_bits(word_len, diff);
}

std::vector<uint16_t> run_chain_search(const std::vector<packed_word>& words, size_t word_len, size_t min_bits, size_t max_bits, size_t max_diff) {
  chain_search_context ctx;
  ctx.chain.reserve(max_chain_length);

  const size_t base_bits = (sizeof(packed_word) + sizeof(uint8_t)) * 8;

  for (uint16_t start = 0; start < words.size(); ++start) {
    ctx.chain.clear();
    ctx.visited.reset();

    ctx.chain.push_back(start);
    ctx.visited.set(start);
    ctx.chain_size_bits = base_bits;

    fill_neighbours(ctx, words, 0, max_diff);

    while (!ctx.chain.empty()) {
      const size_t depth = ctx.chain.size() - 1;

      const packed_word curr = words[ctx.chain.back()];

      // check size
      if (ctx.chain_size_bits > max_bits) {
        const packed_word prev = words[ctx.chain[depth - 1]];
        pop_word(ctx, word_len, prev.diff(curr));
      }

      // success
      if (ctx.chain_size_bits >= min_bits) {
        return ctx.chain;
      }

      auto neighs = std::span<uint16_t, max_word_neighbours>(ctx.neighbours.begin() + depth * max_word_neighbours, max_word_neighbours);
      uint8_t& idx = ctx.neighbour_idx[depth];

      if (idx >= max_word_neighbours || neighs[idx] == word_id_nan) {
        const packed_word prev = words[ctx.chain[ctx.chain.size() - 2]];
        pop_word(ctx, word_len, prev.diff(curr));
        continue;
      }

      const uint16_t next = neighs[idx++];
      const packed_word next_word = words[next];

      push_word(ctx, next, word_len, curr.diff(next_word));
      fill_neighbours(ctx, words, depth + 1, max_diff);
    }
  }

  throw std::runtime_error("Failed to find valid chain");
}

void validate_chain(const std::vector<uint16_t>& chain, const std::vector<packed_word>& words, size_t min_bytes, size_t max_bytes) {
  // encode
  const auto packed = encode_chain_compact_packed(chain, words);

  // size check
  if (packed.size() < min_bytes || packed.size() > max_bytes) {
    throw std::logic_error("Chain size out of bounds: " + std::to_string(packed.size()));
  }

  // decode
  const auto decoded = decode_chain_compact_packed(packed);

  // build reference
  std::vector<packed_word> ref;
  ref.reserve(chain.size());
  for (auto id : chain) {
    ref.emplace_back(words[id]);
  }

  if (decoded != ref) {
    throw std::logic_error("Decoded chain mismatch");
  }
}

inline std::vector<uint16_t> find_compact_chain(const std::vector<packed_word>& words, size_t word_len, size_t min_bytes, size_t max_bytes, size_t max_diff) {
  auto chain = run_chain_search(words, word_len, min_bytes * 8, max_bytes * 8, max_diff);

  validate_chain(chain, words, min_bytes, max_bytes);
  return chain;
}

struct compact_chain_result {
  size_t word_len;
  std::vector<uint16_t> chain;
  std::vector<uint8_t> encoded;
};

compact_chain_result build_and_validate_chain(const std::vector<packed_word>& words, size_t word_len, size_t min_bytes, size_t max_bytes, size_t max_diff) {
  auto chain = run_chain_search(words, word_len, min_bytes * 8, max_bytes * 8, max_diff);

  validate_chain(chain, words, min_bytes, max_bytes);
  auto encoded = encode_chain_compact_packed(chain, words);

  return {.word_len = word_len, .chain = std::move(chain), .encoded = std::move(encoded)};
}

void emit_header_file(const std::vector<compact_chain_result>& chains, const std::string& header_output_path) {
  std::ofstream header_file(header_output_path);
  if (!header_file.is_open()) {
    throw std::runtime_error("Failed to open header output file: " + header_output_path);
  }

  header_file << "#pragma once\n\n";
  header_file << "#include <array>\n";
  header_file << "#include <cstdint>\n\n";
  header_file << "namespace yafth::core::engine_detail::words {\n\n";

  for (const auto& c : chains) {
    header_file << "constexpr static std::array<uint8_t, 128> packed_word_" << c.word_len << " = { ";

    for (size_t i = 0; i < c.encoded.size(); ++i) {
      header_file << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c.encoded[i]);

      if (i + 1 != c.encoded.size()) {
        header_file << ", ";
      }
    }

    header_file << std::dec << " };\n";
  }

  header_file << "\n} // namespace yafth::core::engine_detail::words\n";
}

}  // namespace

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <word dictionary path> <header output path> <random seed>" << '\n';
    return 1;
  }

  try {
    std::string dictionary_path = argv[1];
    std::string header_output_path = argv[2];
    uint64_t seed = std::stoull(argv[3]);

    auto start_time = std::chrono::high_resolution_clock::now();

    auto all_words = load_words(dictionary_path);

    auto load_time = std::chrono::high_resolution_clock::now();
    std::clog << "load took " << std::chrono::duration<double>(load_time - start_time).count() << " seconds\n";

    std::mt19937 rng(seed);

    for (auto& [len, words] : all_words) {
      std::shuffle(words.begin(), words.end(), rng);
      if (words.size() > max_words_count) {
        words.resize(max_words_count);
      }
    }

    auto shuffle_time = std::chrono::high_resolution_clock::now();
    std::clog << "shuffle took " << std::chrono::duration<double>(shuffle_time - load_time).count() << " seconds\n";

    std::vector<compact_chain_result> results;
    results.reserve(9);  // lens 4..12

    for (size_t len = 4; len <= 12; ++len) {
      const auto& words = all_words.at(len);

      results.push_back(build_and_validate_chain(words, len, 124, 128, 3));
    }

    auto chain_time = std::chrono::high_resolution_clock::now();
    std::clog << "chain building and validation took " << std::chrono::duration<double>(chain_time - shuffle_time).count() << " seconds\n";

    emit_header_file(results, header_output_path);

    auto header_time = std::chrono::high_resolution_clock::now();
    std::clog << "header generation took " << std::chrono::duration<double>(header_time - chain_time).count() << " seconds\n";
  } catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return 0;
}