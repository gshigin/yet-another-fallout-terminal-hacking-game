#include "random_generator.hpp"
#include "yafth_engine.hpp"

#include <array>
#include <iostream>
#include <numeric>
#include <array>

using namespace yafth;

int main()
{
    yafth_engine_t yet(LockLevel::Easy, 65, time(0));
    const auto arr = yet.get_chars_stream();
    const auto len = yet.get_word_length();
    const auto count = yet.get_word_count();
    const auto words = yet.get_words();

    std::cout << len << '\n';
    std::cout << count << '\n';
    std::cout << arr[407] << '\n';
    std::cout << words[count-1] << '\n';

    xoroshiro128 rng{1, seed()};
    auto answer = rng.next() % count;

    for(uint32_t i = 0; i < count; ++i)
    {
        int similarity = 0;
        for(uint32_t j = 0; j < len; ++j)
            similarity += (words[i][j] == words[answer][j]);

        std::cout << words[i] << ' ' << similarity << '/' << len << '\n';
    }

    return 0;
}