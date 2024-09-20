#pragma once
//yafth
#include "yafth/random.h"
//stl
#include <cstdint>
#include <array>
#include <string_view>
#include <span>

namespace yafth
{
    enum class LockLevel
    {
        VeryEasy = 0,
        Easy = 1,
        Average = 2,
        Hard = 3,
        VeryHard = 4
    };

    class engine
    {
    public:
        engine(const LockLevel lockLevelSetting_, const uint32_t playerScienceSkill_, const uint64_t seed_);
        engine() : engine(LockLevel::Average, 50, 0) {};
        engine(engine && e) = default;

        std::size_t get_word_count() const {return wordCount;}
        std::size_t get_word_length() const {return wordLength;}
        std::size_t get_attempts() const {return attemptsLeft;}

        auto get_words() const { return std::span(words).subspan(0, wordCount); }
        std::string_view get_term_chars() const {return {term_chars.begin(), term_chars.end()};}

        //std::string print_formatted() const; // debug use only!

        std::string_view look_at(std::size_t i) const;
        //void click_at(std::size_t i);
    private:
        std::size_t set_word_count() noexcept;
        std::size_t set_word_length() noexcept;
        void generate_term_chars() noexcept;
        void generate_words() noexcept;
        void place_words() noexcept;

        random::xoroshiro128 rng;

        const LockLevel lockLevelSetting;
        const uint32_t playerScienceSkill;
        const std::size_t wordLength;
        const std::size_t wordCount;
        const std::size_t answer;

        std::size_t attemptsLeft = 4;

        std::array<std::string_view, 20> words;
        std::array<char, 12 * 20> words_chars;
        std::array<char, 408> term_chars;
    };
} //namespace yafth