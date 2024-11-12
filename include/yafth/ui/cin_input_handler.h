#pragma once

#include <yafth/core/input_handler_interface.h>
#include <yafth/core/types.h>

namespace yafth::ui
{
class cin_input_handler : public core::input_handler_interface
{
public:
    cin_input_handler() {}
    ~cin_input_handler() override {}
    input get_input() override;
};
} // namespace yafth::ui