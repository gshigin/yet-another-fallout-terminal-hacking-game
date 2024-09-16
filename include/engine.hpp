#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <string>
#include <utility>

namespace yafth
{
    // from https://geckwiki.com/index.php?title=Hacking_Word_Count
    enum class LockLevel : uint32_t
    {
        VeryEasy = 0,
        Easy = 1,
        Average = 2,
        Hard = 3,
        VeryHard = 4
    };

    class engine
    {
    private:
        using chars_iter = std::array<char, 408>::iterator;
        using const_chars_iter = std::array<char, 408>::const_iterator;
    public:
        engine(const LockLevel lockLevelSetting_, const uint32_t playerScienceSkill_, const uint64_t seed_);

        uint32_t get_word_count() const {return wordCount;}
        uint32_t get_word_length() const {return wordLength;}
        uint32_t get_attempts() const {return attemptsLeft;}

        const std::vector<std::string> & get_words() const {return words;}
        const std::array<char, 408> & get_chars_stream() const {return chars_stream;}

        const std::string & get_log() const {return log;}

        void print_formatted() const; // debug use only!

        std::pair<const_chars_iter, const_chars_iter> look_at(std::size_t i) const;
        //void click_at(std::size_t i);
    private:
        uint32_t set_word_count() noexcept;
        uint32_t set_word_length() noexcept;
        void generate_chars_stream() noexcept;
        void generate_words() noexcept;
        void place_words() noexcept;

        random::xoroshiro128 rng;

        const LockLevel lockLevelSetting;
        const uint32_t playerScienceSkill;
        const uint32_t wordLength;
        const uint32_t wordCount;
        const uint32_t answer;

        uint32_t attemptsLeft = 4;
        std::string log = "";

        std::vector<std::string> words;
        std::array<char, 408> chars_stream;
        std::vector<chars_iter> words_pointers; // pointers to words in chars_stream
    };
} // namespace yafth