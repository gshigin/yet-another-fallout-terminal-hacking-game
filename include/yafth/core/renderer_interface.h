#pragma once

#include <yafth/core/types.h>

namespace yafth::core
{
class renderer_interface
{
public:
    virtual void initialize(const state &) = 0;
    virtual void display(const state &) = 0;
    virtual ~renderer_interface() = default;
};
} // namespace yafth::core