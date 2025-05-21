// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once
// yafth
#include <yafth/core/types.h>
// stl
#include <functional>
#include <memory>

namespace yafth::ui
{
class user_interface
{
    using fcallback_t = std::function<yafth::state(yafth::input)>;

  public:
    user_interface(fcallback_t callback, std::uint64_t seed);
    ~user_interface();

    void run();

  private:
    struct impl;

    std::unique_ptr<impl> pimpl_;
};
} // namespace yafth::ui
