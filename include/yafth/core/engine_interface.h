// Copyright 2024 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once

#include <yafth/core/types.h>

namespace yafth::core
{
class engine_interface
{
public:
    virtual state process_input(input input) = 0;
    virtual ~engine_interface() = default;
};
}