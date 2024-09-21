#include "yafth/engine.h"

#include <array>
#include <numeric>
#include <array>

#include <iostream>

namespace
{
    void print_formatted(const yafth::engine & e)
    {
        constexpr uint32_t columns = 2;
        constexpr uint32_t rows = 17;
        constexpr uint32_t row_length = 12;

        const auto & term_chars = e.get_term_chars();

        for(uint32_t i = 0; i < columns * rows; ++ i)
        {
            const uint32_t start = (i % 2) * (rows * row_length) + (i / 2) * 12;
            std::cout << ( i == 0 ? "  " : ( start < 100 ? " " : "" ) ) << ( start ) << ' ';
            for(auto j = start; j < start + row_length; ++j) std::cout << term_chars[j];
            std::cout << ( (i % 2) ? '\n' : ' ' );
        }
        std::cout << '\n';
    }
}

int main()
{
    yafth::engine y(yafth::LockLevel::Easy, 65, 0);
    yafth::engine yet(std::move(y));
 
    std::cout << "size : " << sizeof(yet) << " bytes\n";
    std::cout << "words : ";
    const auto & words = yet.get_words();
    std::cout << words.size() << '\n';
    for(const auto& word : words) std::cout << word << " ";
    std::cout << '\n';
    for(auto c : yet.get_term_chars()) std::cout << c;
    std::cout << "\n";
    ::print_formatted( yet );

    char c;
    int x; 
    while(std::cin >> c >> x)
    {    
        if( x < 0 || x >= 408) return 0;

        if(c == 'l')
        {
            auto sv = yet.look_at(x);
            std::cout << "You look at : " << sv << '\n';
        }
        if(c == 'c')
        {
            std::string substr{yet.look_at(x)};
            auto response = yet.click_at(x);

            std::cout << ">" << substr << '\n';

            switch (response.state) {
            case yafth::ClickState::Error:
                std::cout << ">Error\n";
                break;
            case yafth::ClickState::AllowanceReplenished:
                std::cout << ">Allowance Replenished\n";
                break;
            case yafth::ClickState::DudRemoved:
                std::cout << ">Dud Removed\n";
                break;
            case yafth::ClickState::EntryDenied:
                std::cout << ">Entry Denied\n";
                if (response.match) 
                {
                    std::cout << *response.match << "/" << yet.get_word_length() << " correct\n";
                }
                break;
            case yafth::ClickState::ExactMatch:
                std::cout << ">Exact Match!\n";
                return 0;
            }

            for(const auto& word : yet.get_words()) std::cout << word << " ";
            std::cout << '\n';
            ::print_formatted( yet );
        }
        if(c == 'a')
        {
            std::cout << "Attempts : " << yet.get_attempts() << '\n';
        }
        

    } while (true);

    return 0;
}