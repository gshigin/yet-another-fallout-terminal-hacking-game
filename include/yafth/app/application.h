// Copyright 2024 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once

#include <yafth/core/engine.h>
#include <yafth/core/types.h>
#include <yafth/ui/user_interface.h>

#include <ftxui/component/screen_interactive.hpp>

namespace yafth::app
{
class application
{
  public:
    application(yafth::args args);

    void run();

  private:
    ftxui::ScreenInteractive screen_;
    yafth::core::engine engine_;
    yafth::ui::user_interface ui;

    bool game_over = false;
};
} // namespace yafth::app