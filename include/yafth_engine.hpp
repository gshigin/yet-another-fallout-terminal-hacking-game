#ifndef YAFTH_ENGINE_H
#define YAFTH_ENGINE_H

#include <cstdint>
#include <vector>
#include <array>
#include <string>

namespace yafth
{
    // from https://geckwiki.com/index.php?title=Hacking_Word_Count
    enum class LockLevel
    {
        VeryEasy = 0,
        Easy = 1,
        Average = 2,
        Hard = 3,
        VeryHard = 4
    };

    class yafth_engine_t
    {
    public:
        yafth_engine_t(const LockLevel lockLevelSetting_, const uint32_t playerScienceSkill_, const uint64_t seed_);

        uint32_t get_word_count() const {return wordCount;}
        uint32_t get_word_length() const {return wordLength;}
        uint32_t get_attempts() const {return attemptsLeft;}

        const std::vector<std::string> & get_words() const {return words;}
        const std::array<char, 408> & get_chars_stream() const {return chars_stream;}

        const std::string & get_log() const {return log;}
    private:
        uint32_t set_word_count();
        uint32_t set_word_length();
        void generate_chars_stream();
        void generate_words();
        void place_words();

        xoroshiro128 rng;

        const LockLevel lockLevelSetting;
        const uint32_t playerScienceSkill;
        const uint32_t wordLength;
        const uint32_t wordCount;
        const uint32_t answer;

        uint32_t attemptsLeft = 4;
        std::string log = "";

        std::vector<std::string> words;
        std::array<char, 408> chars_stream;
        std::vector<decltype(chars_stream.begin())> words_pointers; // pointers to words in chars_stream
    };
} // namespace yafth

#endif // YAFTH_ENGINE_H