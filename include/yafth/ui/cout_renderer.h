#pragma once

#include <yafth/core/renderer_interface.h>
#include <yafth/core/types.h>

namespace yafth::ui
{
class cout_renderer : public core::renderer_interface
{
public:
    cout_renderer() {}
    ~cout_renderer() override {}
    void display(const state &) override;
};
} // namespace yafth::ui