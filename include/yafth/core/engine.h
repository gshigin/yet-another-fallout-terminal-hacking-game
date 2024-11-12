#pragma once
// yafth
#include <yafth/core/types.h>
#include <yafth/util/random.h>
// stl
#include <array>
#include <bitset>
#include <cstdint>
#include <optional>
#include <string_view>

namespace yafth::core
{
enum class lock_level
{
    very_easy = 0,
    easy = 1,
    average = 2,
    hard = 3,
    very_hard = 4
};

enum class click_result
{
    error, // when clicking free symbol
    dud_removed, // when clicked brackets, removes one wrong answer
    allowance_replenished, // when clicked brackets, restores attempts
    entry_denied, // when clicked wrong answer
    lockout_in_progress, // lock after all attemps are over
    exact_match // when guessed the answer
};

struct click_status
{
    click_result state;
    std::optional<std::size_t> match;
};

class engine
{
    enum class engine_state
    {
        running,
        done
    };
public:
    engine(const lock_level lock_level_setting_, const uint32_t player_science_skill_, const uint64_t seed_);
    engine()
        : engine(lock_level::average, 50, 0) {};

    state process_input(input input);
private:
    highlight look_at(std::size_t i) const;
    click_status click_at(std::size_t i);

    std::size_t set_word_count() noexcept;
    std::size_t set_word_length() noexcept;
    void generate_term_chars() noexcept;
    void generate_words() noexcept;

    random::xoroshiro128 rng;

    const lock_level lock_level_setting;
    const uint32_t player_science_skill;
    const std::size_t word_length;
    const std::size_t word_count;
    const std::size_t answer;

    std::size_t attempts_left = 4;
    std::size_t words_left;

    std::array<std::size_t, 20> words;
    std::array<char, 408> term_chars;

    std::bitset<408> used_bracket;

    engine_state internal_state = engine_state::running;
};
} // namespace yafth::core