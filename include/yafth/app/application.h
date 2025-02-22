// Copyright 2024 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once
#include <yafth/core/types.h>

#include <memory>

namespace yafth::app
{
class application
{
  public:
    application(yafth::args args);
    ~application();

    application(application const &) = delete;
    auto operator=(application const &) -> application & = delete;

    application(application &&) noexcept = delete;
    auto operator=(application &&) noexcept -> application & = delete;

    void run();

  private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};
} // namespace yafth::app