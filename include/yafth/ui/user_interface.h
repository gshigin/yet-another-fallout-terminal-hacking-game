// Copyright 2024 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once

#include <yafth/core/types.h>
#include <yafth/util/random.h>

#include <ftxui/component/component.hpp>

#include <string>
#include <vector>

namespace yafth::ui
{
class user_interface
{
    using fcallback_t = std::function<yafth::state(yafth::input)>;

  public:
    user_interface(fcallback_t callback, std::uint64_t seed);

    auto create() -> ftxui::Component;

  private:
    void update_internals_();

    fcallback_t callback_;
    std::string last_loocked_ = "";
    state terminal_state_ = {};
    std::uint64_t start_hex_ = 0;
    std::vector<ftxui::Element> log_ = {};
};
} // namespace yafth::ui
