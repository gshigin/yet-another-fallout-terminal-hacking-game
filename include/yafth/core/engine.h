// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#pragma once
// yafth
#include <yafth/core/types.h>
#include <yafth/util/random.h>
// stl
#include <cstdint>
#include <memory>

namespace yafth::core
{
class engine
{
  public:
    engine(lock_level lock_level_setting, uint32_t player_science_skill, uint64_t seed) noexcept;
    engine() noexcept : engine(lock_level::average, 50, 0){};

    ~engine();

    engine(engine const &) = delete;
    auto operator=(engine const &) -> engine & = delete;

    engine(engine &&) noexcept = delete;
    auto operator=(engine &&) noexcept -> engine & = delete;

    auto process_input(input input) noexcept -> state;

  private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};
} // namespace yafth::core