// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
// yafth
#include <yafth/app/application.h>
#include <yafth/core/engine.h>
#include <yafth/ui/user_interface.h>

namespace yafth::app
{
class application::impl
{
  public:
    explicit impl(yafth::args args);
    void run();

  private:
    yafth::core::engine engine_;
    yafth::ui::user_interface ui_;

    bool game_over_{false};
};

application::impl::impl(yafth::args args)
    : engine_(args.lock, args.science_level, args.seed),
      ui_(
          [this](input inp) {
              if (game_over_)
              {
                  return engine_.process_input(input{input_type::other, {}});
              }

              state current_state = engine_.process_input(inp);

              if (current_state.click_res.has_value() &&
                  (current_state.click_res->state == click_result::exact_match || current_state.click_res->state == click_result::lockout_in_progress))
              {
                  auto res = current_state.click_res->state;
                  if (res == click_result::exact_match || res == click_result::lockout_in_progress)
                  {
                      game_over_ = true;
                  }
              }
              return current_state;
          },
          util::seed(args.seed))

{
}

void application::impl::run()
{
    ui_.run();
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