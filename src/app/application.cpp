// Copyright 2024 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <yafth/app/application.h>
#include <yafth/core/engine.h>
#include <yafth/ui/user_interface.h>

#include <ftxui/component/screen_interactive.hpp>

namespace yafth::app
{
struct application::impl
{
    impl(yafth::args args);
    void run();

    ftxui::ScreenInteractive screen;
    yafth::core::engine engine;
    yafth::ui::user_interface ui;

    bool game_over{false};
};

application::impl::impl(yafth::args args)
    : screen(ftxui::ScreenInteractive::FitComponent()), engine(args.lock, args.science_level, args.seed),
      ui(
          [this](input inp) {
              if (game_over)
              {
                  screen.ExitLoopClosure()();
                  return engine.process_input(input{input_type::other, {}});
              }
              state current_state = engine.process_input(inp);
              if (current_state.click_res.has_value() &&
                  (current_state.click_res->state == click_result::exact_match ||
                   current_state.click_res->state == click_result::lockout_in_progress))
              {
                  // end game on next call
                  game_over = true;
                  screen.PostEvent(ftxui::Event::Custom);
              }
              return current_state;
          },
          util::seed(args.seed))
      
{
}

void application::impl::run()
{
    screen.Loop(ui.create());
}

application::application(yafth::args args) : pimpl_(std::make_unique<impl>(args))
{
}

application::~application() = default;

void application::run()
{
    pimpl_->run();
}

} // namespace yafth::app