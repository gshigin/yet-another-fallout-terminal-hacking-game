// Copyright 2024 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once

#include <yafth/core/types.h>

namespace yafth::core
{
class input_handler_interface
{
public:
    virtual input get_input() = 0;
    virtual ~input_handler_interface() = default;
};

} // namespace yafth::core