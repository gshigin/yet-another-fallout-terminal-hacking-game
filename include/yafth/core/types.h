#pragma once

#include <optional>
#include <string_view>

namespace yafth::core
{
inline namespace types
{

enum class input_type
{
    look,
    click,
    other // treated as nothing happened
};

struct screen_coords
{
    int x;
    int y;
};

struct input
{
    input_type type;
    std::optional<screen_coords> coords;
};

struct highlight
{
    std::size_t first;
    std::size_t last;
};

struct state
{
    std::string_view term_chars; // all current mutable characters
    std::size_t attempts_left;
    std::optional<highlight> highlighted;
};

} // namespace types

} // namespace yafth::core