#include "random_generator.hpp"
#include "yafth_engine.hpp"

#include <array>
#include <numeric>
#include <array>

#include <iostream>

using namespace yafth;

int main()
{
    yafth_engine_t yet(LockLevel::Easy, 65, time(0));
 
    std::cout << sizeof(yet) << '\n';

    return 0;
}