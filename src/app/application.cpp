// Copyright 2024 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <yafth/app/application.h>

#include <ftxui/component/screen_interactive.hpp>

namespace yafth::app
{
application::application(yafth::args args)
    : screen_(ftxui::ScreenInteractive::FitComponent()), engine_(args.lock, args.science_level, args.seed),
      ui(
          [this](input inp) {
              if (game_over)
              {
                  screen_.ExitLoopClosure()();
                  return engine_.process_input(input{input_type::other, {}});
              }
              state current_state = engine_.process_input(inp);
              if (current_state.click_res.has_value() &&
                  (current_state.click_res->state == click_result::exact_match ||
                   current_state.click_res->state == click_result::lockout_in_progress))
              {
                  // end game on next call
                  game_over = true;
                  screen_.PostEvent(ftxui::Event::Custom);
              }
              return current_state;
          },
          util::seed(args.seed))
{
}

void application::run()
{
    screen_.Loop(ui.create());
}
} // namespace yafth::app