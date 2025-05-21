// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once
// yafth
#include <yafth/core/types.h>
// stl
#include <memory>

namespace yafth::app
{
class application
{
  public:
    application(yafth::args args);
    ~application();

    void run();

  private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};
} // namespace yafth::app