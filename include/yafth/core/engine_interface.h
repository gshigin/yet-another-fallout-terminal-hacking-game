#pragma once

#include <yafth/core/types.h>

namespace yafth::core
{
class engine_interface
{
public:
    virtual state process_input(input input) = 0;
    virtual ~engine_interface() = default;
};
}