#pragma once

#include <yafth/core/types.h>

namespace yafth::core
{

class input_handler_interface
{
    virtual input_type get_input() = 0;
    virtual ~input_handler_interface() = default;
};

} // namespace yafth::core