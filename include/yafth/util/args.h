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
    static auto parse(int argc, char *argv[]) -> args;

  private:
    static void show_help_();
    static auto get_next_arg_(int argc, char *argv[], int current_index, const std::string &arg_name) -> std::string;

    static auto handle_science_(const std::string &value) -> std::uint32_t;
    static auto handle_lock_(const std::string &value) -> lock_level;
    static auto handle_seed_(const std::string &value) -> std::uint64_t;
    static void check_defaults_(std::optional<std::uint32_t> &science_level, std::optional<lock_level> &lock,
                                std::optional<std::uint64_t> &seed);
};
} // namespace yafth::util
