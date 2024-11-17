// Copyright 2024 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once

#include <yafth/core/renderer_interface.h>
#include <yafth/core/types.h>
#include <yafth/ui/ftxui_shared_state.h>

#include <memory>

namespace yafth::ui
{
class ftxui_renderer : public core::renderer_interface
{
public:
    ftxui_renderer(std::shared_ptr<ftxui_shared_state> shared_state)
        : shared_state_(shared_state)
    {}
    void initialize(const state & initial_state) override { shared_state_->initialize(initial_state); }
    void display(const state & current_state) override { shared_state_->display(current_state); };
    ~ftxui_renderer() override {}
private:
    std::shared_ptr<ftxui_shared_state> shared_state_;
};

} // namespace yafth::ui