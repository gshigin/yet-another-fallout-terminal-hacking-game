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
};

application::impl::impl(yafth::args args)
    : engine_(args.lock, args.science_level, args.seed), ui_(
                                                             [this](input inp) {
                                                                 state current_state = engine_.process_input(inp);
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