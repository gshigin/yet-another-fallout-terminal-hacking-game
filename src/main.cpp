#include "random.hpp"
#include "engine.hpp"

#include <array>
#include <numeric>
#include <array>

#include <iostream>

int main()
{
    yafth::engine yet(yafth::LockLevel::Easy, 65, time(0));
 
    std::cout << sizeof(yet) << '\n';

    return 0;
}