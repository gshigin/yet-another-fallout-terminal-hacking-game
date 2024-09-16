#include "random.hpp"
#include "engine.hpp"

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
    yet.print_formatted();

    {
        const auto & [b, e] = yet.look_at(3);
        std::cout << "You look at : ";
        for(auto it = b; it != e; ++it)
        {
            std::cout << *it;
        }
        std::cout << '\n';
    }

    {
        const auto & [b, e] = yet.look_at(4);
        std::cout << "You look at : ";
        for(auto it = b; it != e; ++it)
        {
            std::cout << *it;
        }
        std::cout << '\n';
    }

    {
        const auto & [b, e] = yet.look_at(12);
        std::cout << "You look at : ";
        for(auto it = b; it != e; ++it)
        {
            std::cout << *it;
        }
        std::cout << '\n';
    }

    {
        const auto & [b, e] = yet.look_at(372);
        std::cout << "You look at : ";
        for(auto it = b; it != e; ++it)
        {
            std::cout << *it;
        }
        std::cout << '\n';
    }

    return 0;
}