#pragma once

#include <yafth/core/types.h>

namespace yafth::core
{
class input_handler_interface
{
public:
    virtual input get_input() = 0;
    virtual ~input_handler_interface() = default;
};

} // namespace yafth::core