#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

constexpr int MAX_WORDS = 65536;

int diff_packed_words(uint64_t a, uint64_t b)
{
    assert(((a >> 60) & 0xF) == ((b >> 60) & 0xF));

    uint64_t x = (a ^ b);
    int len = static_cast<int>((a >> 60) & 0xF);
    x >>= (12 - len) * 5;

    int diff = 0;
    for (int i = 0; i < len; ++i)
    {
        if ((x & 0x1F))
            ++diff;
        x >>= 5;
    }
    return diff;
}

std::vector<std::pair<int, char>> get_packed_words_diffs(uint64_t a, uint64_t b)
{
    assert(((a >> 60) & 0xF) == ((b >> 60) & 0xF));

    int len = (a >> 60) & 0xF;
    int shift = (12 - len) * 5;

    a >>= shift;
    b >>= shift;

    std::vector<std::pair<int, char>> diffs;

    for (int pos = 0; pos < len; ++pos)
    {
        int char_a = (a >> (5 * (len - pos - 1))) & 0x1F;
        int char_b = (b >> (5 * (len - pos - 1))) & 0x1F;

        if (char_a != char_b)
        {
            diffs.emplace_back(pos, 'a' + char_b);
        }
    }

    return diffs;
}

uint64_t pack_word(std::string_view word)
{
    uint64_t result = word.size() & 0xF;
    for (char c : word)
    {
        result <<= 5;
        result |= (c - 'a') & 0x1F;
    }

    int padding = (12 - word.size()) * 5;
    result <<= padding;
    return result;
}

std::string unpack_word(uint64_t packed)
{
    int len = (packed >> 60) & 0xF;
    std::string word(len, 'a');

    int shift = 60 - 5;
    for (char &c : word)
    {
        c += (packed >> shift) & 0x1F;
        shift -= 5;
    }
    return word;
}

std::string_view getword(const char *&_str, const char *end)
{
    const char *str = _str;
    while (*str == '\n' || *str == '\r')
    {
        if (++str == end)
            return (_str = str), std::string_view{};
    }

    const char *start = str;
    while (str != end && *str != '\n' && *str != '\r')
        ++str;

    _str = str;
    return std::string_view{start, str};
}

std::unordered_map<uint32_t, std::vector<uint64_t>> load_words(const std::string &filename)
{
    std::ifstream in(filename);
    std::vector<char> buf;

    in.seekg(0, std::ios::end);
    buf.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&buf[0], buf.size());

    const char *str = &buf[0];
    const char *strEnd = str + buf.size();

    std::string_view word;
    std::unordered_map<uint32_t, std::vector<uint64_t>> ans;

    while (!(word = getword(str, strEnd)).empty())
    {
        const auto size = word.size();
        if (size < 4 || size > 12)
            continue;
        ans[size].push_back(pack_word(word));
    }

    return ans;
}

bool flexible_dfs(uint16_t current_idx, const std::vector<uint64_t> &words, std::bitset<MAX_WORDS> &visited, std::vector<uint16_t> &path, int target_length,
                  int max_diff)
{
    if ((int)path.size() >= target_length)
        return true;

    std::vector<std::pair<int, uint16_t>> candidates;

    const uint64_t current_word = words[current_idx];

    for (uint16_t idx = 0; idx < words.size(); ++idx)
    {
        if (visited[idx])
            continue;

        int diff = diff_packed_words(current_word, words[idx]);
        if (diff > 0 && diff <= max_diff)
            candidates.emplace_back(diff, idx);
    }

    // std::sort(candidates.begin(), candidates.end());

    for (const auto &[diff, idx] : candidates)
    {
        visited[idx] = true;
        path.push_back(idx);
        if (flexible_dfs(idx, words, visited, path, target_length, max_diff))
            return true;
        path.pop_back();
        visited[idx] = false;
    }

    return false;
}

// ------------------------- BitWriter -------------------------
class BitWriter
{
    std::vector<uint8_t> &out;
    uint8_t buffer = 0;
    int bits_filled = 0;

  public:
    BitWriter(std::vector<uint8_t> &output) : out(output)
    {
    }

    void write_bits(uint32_t value, int count)
    {
        while (count > 0)
        {
            int to_write = std::min(8 - bits_filled, count);
            buffer <<= to_write;
            buffer |= (value >> (count - to_write)) & ((1 << to_write) - 1);
            bits_filled += to_write;
            count -= to_write;

            if (bits_filled == 8)
            {
                out.push_back(buffer);
                buffer = 0;
                bits_filled = 0;
            }
        }
    }

    void flush()
    {
        if (bits_filled > 0)
        {
            buffer <<= (8 - bits_filled);
            out.push_back(buffer);
            buffer = 0;
            bits_filled = 0;
        }
    }
};

// ------------------------- BitReader -------------------------
class BitReader
{
    const std::vector<uint8_t> &in;
    size_t byte_index = 0;
    int bits_left = 0;
    uint8_t buffer = 0;

  public:
    BitReader(const std::vector<uint8_t> &input) : in(input)
    {
    }

    uint32_t read_bits(int count)
    {
        uint32_t result = 0;
        while (count > 0)
        {
            if (bits_left == 0)
            {
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
int get_position_bits(int word_len)
{
    if (word_len <= 4)
        return 2;
    if (word_len <= 8)
        return 3;
    if (word_len <= 16)
        return 4;
    return 5;
}

// ------------------------- Encoding -------------------------
std::vector<uint8_t> encode_chain_compact_packed(const std::vector<uint16_t> &chain, const std::vector<uint64_t> &words)
{
    if (chain.empty())
        throw std::invalid_argument("Empty chain");

    const int word_len = (words[chain[0]] >> 60) & 0xF;
    const int pos_bits = get_position_bits(word_len);

    std::vector<uint8_t> buffer;
    uint64_t packed = words[chain[0]];
    for (int i = 7; i >= 0; --i)
        buffer.push_back((packed >> (i * 8)) & 0xFF);

    buffer.push_back(static_cast<uint8_t>(chain.size() - 1));

    BitWriter writer(buffer);
    for (size_t i = 1; i < chain.size(); ++i)
    {
        uint64_t prev = words[chain[i - 1]];
        uint64_t curr = words[chain[i]];

        if (((prev >> 60) & 0xF) != ((curr >> 60) & 0xF))
            throw std::invalid_argument("Word lengths mismatch");

        auto diffs = get_packed_words_diffs(prev, curr);

        if (diffs.empty() || diffs.size() > 3)
            throw std::runtime_error("Invalid number of diffs");

        uint8_t diff_code = diffs.size() == 1 ? 0b01 : diffs.size() == 2 ? 0b10 : 0b11;
        writer.write_bits(diff_code, 2);

        for (const auto &[pos, c] : diffs)
        {
            writer.write_bits(pos, pos_bits);
            writer.write_bits(c - 'a', 5);
        }
    }

    writer.flush();
    return buffer;
}

// ------------------------- Decoding -------------------------
std::vector<uint64_t> decode_chain_compact_packed(const std::vector<uint8_t> &data)
{
    if (data.size() < 9)
        throw std::runtime_error("Too short");

    uint64_t packed = 0;
    for (int i = 0; i < 8; ++i)
        packed = (packed << 8) | data[i];

    int word_len = (packed >> 60) & 0xF;
    const int pos_bits = get_position_bits(word_len);

    BitReader reader(data);
    reader.read_bits(64); // skip first 64 bits (word)
    int steps = reader.read_bits(8);

    std::vector<uint64_t> chain = {packed};
    for (int i = 0; i < steps; ++i)
    {
        uint64_t current = chain.back();
        int code = reader.read_bits(2);
        int count = code == 0b01 ? 1 : code == 0b10 ? 2 : 3;

        int shift_offset = (12 - word_len) * 5;
        uint64_t body = current >> shift_offset; // обнуляем паддинги

        for (int j = 0; j < count; ++j)
        {
            int pos = reader.read_bits(pos_bits);
            int letter = reader.read_bits(5);

            int bitpos = (word_len - pos - 1) * 5;
            body &= ~(uint64_t(0x1F) << bitpos);  // сбросить старую букву
            body |= (uint64_t(letter) << bitpos); // записать новую букву
        }

        uint64_t new_word = (uint64_t(word_len) << 60) | (body << shift_offset);
        chain.push_back(new_word);
    }

    return chain;
}

std::string generate_header_file(std::unordered_map<uint32_t, std::vector<uint64_t>> &all_words, uint32_t seed, const std::string &header_output_path)
{
    std::mt19937 rng(seed);
    std::ofstream header_file(header_output_path);

    if (!header_file.is_open())
    {
        std::cerr << "Failed to open header output file" << std::endl;
        return "Error opening header file";
    }

    header_file << "#pragma once\n\n";

    header_file << "#include <array>\n";
    header_file << "#include <cstdint>\n";

    header_file << "namespace yafth::core::engine_detail::words {\n\n";

    std::bitset<65536> visited;
    std::vector<uint16_t> path;
    std::vector<uint8_t> compact_path;

    for (int word_length = 4; word_length <= 12; ++word_length)
    {
        std::cout << "Chain of length " << word_length << " generation started\n";

        auto &words = all_words[word_length]; // Assuming that words are loaded by length
        std::shuffle(words.begin(), words.end(), rng);

        for (int target_length = 100; target_length != 0; --target_length)
        {
            size_t cnt = 0;

            for (uint16_t start_idx = 0; start_idx < words.size(); ++start_idx)
            {
                visited.reset();
                path.clear();
                compact_path.clear();

                visited[start_idx] = true;
                path.push_back(start_idx);

                if (flexible_dfs(start_idx, words, visited, path, target_length, 3))
                {
                    compact_path = encode_chain_compact_packed(path, words);

                    if (compact_path.size() == 128)
                        break;

                    if (compact_path.size() > 140)
                        break;

                    ++cnt;
                    if (cnt == 25)
                        break;
                }
            }
            if (compact_path.size() == 128)
            {
                auto decoded = decode_chain_compact_packed(compact_path);
                std::cout << "Chain of length " << word_length << " generated\n";

                std::vector<uint64_t> ref;
                ref.reserve(path.size());
                for (auto id : path)
                {
                    ref.emplace_back(words[id]);
                }

                if (ref != decoded)
                {
                    throw std::runtime_error("Chain validation error!");
                }
                else
                {
                    std::cout << "Chain of length " << word_length << " validated\n";
                }

                std::cout << "Chain of length " << word_length << " : ";
                for (auto w : decoded)
                {
                    std::cout << unpack_word(w) << ' ';
                }
                std::cout << '\n';

                header_file << "constexpr static std::array<uint8_t, 128> packed_word_" << std::dec << word_length << std::hex << " = { ";

                for (size_t i = 0; i < compact_path.size(); ++i)
                {
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

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <word dictionary path> <header output path> <random seed>" << std::endl;
        return 1;
    }

    std::string dictionary_path = argv[1];
    std::string header_output_path = argv[2];
    uint32_t seed = std::stoi(argv[3]);

    // Load words from dictionary (use the `load_words` function from your original code)
    auto all_words = load_words(dictionary_path);

    // Generate header file
    std::string result = generate_header_file(all_words, seed, header_output_path);

    std::cout << result << std::endl;
    return 0;
}