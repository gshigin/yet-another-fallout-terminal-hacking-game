#include "yafth/engine.h"

#include <array>
#include <numeric>
#include <array>

#include <iostream>

int main()
{
    yafth::engine yet(yafth::LockLevel::Easy, 65, 0);
 
    std::cout << "size : " << sizeof(yet) << " bytes\n";
    std::cout << "words : ";
    for(const auto& word : yet.get_words()) std::cout << word << " ";
    std::cout << '\n';
    for(auto c : yet.get_chars_stream()) std::cout << c;
    std::cout << "\n";
    std::cout << yet.print_formatted() << '\n';

    do
    {
        int x; 
        std::cin >> x;
        if( x < 0 || x >= 408) return 0;

        const auto & [b, e] = yet.look_at(x);
        std::cout << "You look at : ";
        for(auto it = b; it != e; ++it)
        {
            std::cout << *it;
        }
        std::cout << '\n';

    } while (true);

    return 0;
}