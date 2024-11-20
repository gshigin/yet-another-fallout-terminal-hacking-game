// Copyright 2024 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <yafth/core/types.h>
#include <yafth/util/args.h>
#include <yafth/util/random.h>

#include <chrono>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>

namespace yafth::util
{

args argparser::parse(int argc, char * argv[])
{
    std::optional<std::uint32_t> science_level;
    std::optional<lock_level> lock;
    std::optional<std::uint64_t> seed;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help")
        {
            show_help_();
            exit(0);
        } else if (arg == "-s" || arg == "--science")
        {
            if (science_level.has_value())
            {
                throw std::invalid_argument("--science specified multiple times.");
            }
            science_level = handle_science_(get_next_arg_(argc, argv, ++i, "--science"));
        } else if (arg == "-l" || arg == "--lock")
        {
            if (lock.has_value())
            {
                throw std::invalid_argument("--lock specified multiple times.");
            }
            lock = handle_lock_(get_next_arg_(argc, argv, ++i, "--lock"));
        } else if (arg == "--seed")
        {
            if (seed.has_value())
            {
                throw std::invalid_argument("--seed specified multiple times.");
            }
            seed = handle_seed_(get_next_arg_(argc, argv, ++i, "--seed"));
        } else
        {
            throw std::invalid_argument("Unknown argument: " + arg);
        }
    }

    check_defaults_(science_level, lock, seed);

    return { science_level.value(), lock.value(), seed.value() };
}

void argparser::show_help_()
{
    std::cout << "Usage: yafth [options]\n"
              << "Options:\n"
              << "  -h, --help              Show this help message\n"
              << "  -s, --science VALUE     Science level (0-100)\n"
              << "  -l, --lock VALUE        Lock level (0-4 or very_easy, easy, average, hard, very_hard)\n"
              << "  --seed VALUE            Optional seed for random generator\n";
}

std::string argparser::get_next_arg_(int argc, char * argv[], int current_index, const std::string & arg_name)
{
    if (current_index >= argc)
    {
        throw std::invalid_argument(arg_name + " requires a value.");
    }
    return argv[current_index];
}

std::uint32_t argparser::handle_science_(const std::string & value)
{
    int level = 0;
    try
    {
        level = std::stoi(value);
        if (level < 0 || level > 100)
        {
            throw std::invalid_argument("");
        }
    } catch (...)
    {
        throw std::invalid_argument("--science must be a number between 0 and 100.");
    }
    return level;
}

lock_level argparser::handle_lock_(const std::string & value)
{
    static const std::unordered_map<std::string, lock_level> valid_levels = {
        { "0",         lock_level::very_easy },
        { "1",         lock_level::easy      },
        { "2",         lock_level::average   },
        { "3",         lock_level::hard      },
        { "4",         lock_level::very_hard },
        { "very_easy", lock_level::very_easy },
        { "easy",      lock_level::easy      },
        { "average",   lock_level::average   },
        { "hard",      lock_level::hard      },
        { "very_hard", lock_level::very_hard }
    };
    if (valid_levels.find(value) == valid_levels.end())
    {
        throw std::invalid_argument(
            "--lock must be a number (0-4) or a string (very_easy, easy, average, hard, very_hard).");
    }
    return valid_levels.find(value)->second;
}

std::uint64_t argparser::handle_seed_(const std::string & value)
{
    std::uint64_t seed;
    try
    {
        seed = std::stoull(value);
    } catch (...)
    {
        throw std::invalid_argument("--seed must be convertible to unsigned long long.");
    }
    return seed;
}

void argparser::check_defaults_(std::optional<std::uint32_t> & science_level, std::optional<lock_level> & lock,
                                std::optional<std::uint64_t> & seed)
{
    if (!seed.has_value())
    {
        // get current time
        seed = static_cast<std::uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        // do magic
        seed = util::seed(util::seed(util::seed(seed.value())));
    }

    if (science_level.has_value())
    {
        if (lock.has_value())
        {
            std::uint32_t science_skill_required =
                *lock == lock_level::very_easy ? 15 : 25 * static_cast<std::uint32_t>(*lock);
            if (!(*science_level >= science_skill_required))
            {
                throw std::invalid_argument("A Science skill of " + std::to_string(science_skill_required) +
                                            " is required to hack this terminal. >:(");
            }
        } else
        {
            if (*science_level < 15)
            {
                throw std::invalid_argument("Not enought Science skill for even Very Easy terminal. :(");
            } else
            {
                if (*science_level < 25)
                {
                    lock = lock_level::very_easy;
                } else
                {
                    lock = static_cast<lock_level>(100 / (*science_level));
                }
            }
        }
    } else
    {
        if (lock.has_value())
        {
            if (*lock == lock_level::very_easy)
            {
                science_level = 15;
            } else
            {
                science_level = 25 * static_cast<std::uint32_t>(*lock);
            }
        } else
        {
            science_level = 50;
            lock = lock_level::average;
        }
    }
}

} // namespace yafth::util
