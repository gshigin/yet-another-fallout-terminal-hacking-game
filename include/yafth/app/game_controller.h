// Copyright 2024 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once

#include <yafth/core/engine.h>
#include <yafth/core/input_handler_interface.h>
#include <yafth/core/renderer_interface.h>
#include <yafth/core/types.h>

#include <memory>

namespace yafth::app
{
class game_controller
{
public:
    game_controller(std::unique_ptr<core::engine_interface> eng_ptr, std::unique_ptr<core::renderer_interface> rend_ptr,
                    std::unique_ptr<core::input_handler_interface> input_ptr);

    void run();
private:
    std::unique_ptr<core::engine_interface> eng_ptr_;
    std::unique_ptr<core::renderer_interface> rend_ptr_;
    std::unique_ptr<core::input_handler_interface> input_ptr_;
};
} // namespace yafth::app