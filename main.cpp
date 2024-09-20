#include "yafth/engine.h"

#include <array>
#include <numeric>
#include <array>

#include <iostream>

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
    //std::cout << yet.print_formatted() << '\n';

    int x; 
    while(std::cin >> x)
    {    
        if( x < 0 || x >= 408) return 0;

        auto sv = yet.look_at(x);
        std::cout << "You look at : " << sv << '\n';

    } while (true);

    return 0;
}