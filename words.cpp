#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

int letter_difference(const std::string &a, const std::string &b)
{
    int diff = 0;
    for (size_t i = 0; i < a.size(); ++i)
        if (a[i] != b[i])
            ++diff;
    return diff;
}

std::vector<std::string> load_words(const std::string &filename, int word_length)
{
    std::ifstream in(filename);
    std::string word;
    std::vector<std::string> words;

    if (!in)
    {
        std::cerr << "Error: Unable to open file: " << filename << "\n";
        return words;
    }

    while (in >> word)
    {
        if ((int)word.length() == word_length)
            words.push_back(word);
    }

    return words;
}

bool flexible_dfs(const std::string &current, const std::vector<std::string> &all_words, std::unordered_set<std::string> &visited,
                  std::vector<std::string> &path, int target_length, int max_diff)
{

    if ((int)path.size() >= target_length)
        return true;

    std::vector<std::pair<int, std::string>> candidates;

    for (const auto &word : all_words)
    {
        if (visited.count(word))
            continue;
        int diff = letter_difference(current, word);
        if (diff > 0 && diff <= max_diff)
            candidates.emplace_back(diff, word);
    }

    std::sort(candidates.begin(), candidates.end());

    for (const auto &[diff, word] : candidates)
    {
        visited.insert(word);
        path.push_back(word);
        if (flexible_dfs(word, all_words, visited, path, target_length, max_diff))
            return true;
        path.pop_back();
        visited.erase(word);
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

uint64_t pack_word(const std::string &word)
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

// ------------------------- Encoding -------------------------
std::vector<uint8_t> encode_chain_compact(const std::vector<std::string> &chain)
{
    if (chain.empty())
        throw std::invalid_argument("Empty chain");

    const int word_len = chain[0].size();
    const int pos_bits = get_position_bits(word_len);

    std::vector<uint8_t> buffer;
    uint64_t packed = pack_word(chain[0]);
    for (int i = 7; i >= 0; --i)
        buffer.push_back((packed >> (i * 8)) & 0xFF);

    buffer.push_back(static_cast<uint8_t>(chain.size() - 1));

    BitWriter writer(buffer);
    for (size_t i = 1; i < chain.size(); ++i)
    {
        const std::string &prev = chain[i - 1];
        const std::string &curr = chain[i];
        if (curr.size() != word_len)
            throw std::invalid_argument("Word lengths mismatch");

        std::vector<std::pair<int, char>> diffs;
        for (int j = 0; j < word_len; ++j)
            if (prev[j] != curr[j])
                diffs.emplace_back(j, curr[j]);

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
std::vector<std::string> decode_chain_compact(const std::vector<uint8_t> &data)
{
    if (data.size() < 9)
        throw std::runtime_error("Too short");

    uint64_t packed = 0;
    for (int i = 0; i < 8; ++i)
        packed = (packed << 8) | data[i];

    std::string first_word = unpack_word(packed);
    const int word_len = first_word.size();
    const int pos_bits = get_position_bits(word_len);

    BitReader reader(data);
    reader.read_bits(64); // skip word bits
    int steps = reader.read_bits(8);

    std::vector<std::string> chain = {first_word};
    for (int i = 0; i < steps; ++i)
    {
        std::string current = chain.back();
        int code = reader.read_bits(2);
        int count = code == 0b01 ? 1 : code == 0b10 ? 2 : 3;

        for (int j = 0; j < count; ++j)
        {
            int pos = reader.read_bits(pos_bits);
            char c = 'a' + reader.read_bits(5);
            current[pos] = c;
        }
        chain.push_back(current);
    }

    return chain;
}

int main()
{
    std::string filename = "words_alpha.txt";
    int word_length = 12;
    int target_chain_length = 50;
    int max_diff = 3;
    int num_chains = 1;

    std::vector<std::string> words = load_words(filename, word_length);
    if (words.empty())
    {
        std::cerr << "No words loaded.\n";
        return 1;
    }

    std::cout << "Loaded " << words.size() << " words of length " << word_length << ".\n";

    std::vector<std::string> chain;
    std::unordered_set<std::string> used_words;

    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::shuffle(words.begin(), words.end(), rng);

    for (const std::string &start : words)
    {
        if (used_words.count(start))
            continue;

        std::unordered_set<std::string> visited{start};
        std::vector<std::string> path{start};

        if (flexible_dfs(start, words, visited, path, target_chain_length, max_diff))
        {
            chain = path;
            for (const auto &w : path)
                used_words.insert(w);

            break;
        }
    }

    int chain_size = 0;
    for (auto &word : chain)
    {
        chain_size += sizeof(word) + word.size();
    }
    chain_size += sizeof(chain);

    std::cout << "Uncopressed chain size : " << chain_size << '\n';

    auto compressed_cahin = encode_chain_compact(chain);

    chain_size = sizeof(compressed_cahin) + compressed_cahin.size();

    std::cout << "Copressed chain size : " << chain_size << '\n';

    auto decoded_chain = decode_chain_compact(compressed_cahin);

    if (chain != decoded_chain)
    {
        std::cout << "Decoded chain not qual to original!\n";
    }

    for (auto byte : compressed_cahin)
    {

        std::cout << "0x" << std::uppercase << std::setw(2) << std::hex << std::uintptr_t(byte) << std::setfill('0') << ", ";
    }
    std::cout << '\n';

    return 0;
}
