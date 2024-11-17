// Copyright 2024 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once

#include <yafth/core/input_handler_interface.h>
#include <yafth/core/types.h>
#include <yafth/ui/ftxui_shared_state.h>

#include <memory>

namespace yafth::ui
{

class ftxui_input_handler : public core::input_handler_interface
{
public:
    ftxui_input_handler(std::shared_ptr<ftxui_shared_state> shared_state)
        : shared_state_(shared_state)
    {}
    ~ftxui_input_handler() override {}
    input get_input() override { return shared_state_->get_input(); }
private:
    std::shared_ptr<ftxui_shared_state> shared_state_;
};
} // namespace yafth::ui