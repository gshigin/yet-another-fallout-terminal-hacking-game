// Copyright 2024 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once

#include <yafth/core/types.h>

#include <optional>
#include <string>

namespace yafth::util
{
class argparser
{
public:
    static args parse(int argc, char * argv[]);
private:
    static void show_help_();
    static std::string get_next_arg_(int argc, char * argv[], int current_index, const std::string & arg_name);

    static std::uint32_t handle_science_(const std::string & value);
    static lock_level handle_lock_(const std::string & value);
    static std::uint64_t handle_seed_(const std::string & value);
    static void check_defaults_(std::optional<std::uint32_t> & science_level, std::optional<lock_level> & lock,
                                std::optional<std::uint64_t> & seed);
};
} // namespace yafth::util
