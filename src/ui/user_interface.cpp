// Copyright 2024 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <yafth/ui/user_interface.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

namespace yafth::ui
{
user_interface::user_interface(const fcallback_t &callback, std::uint64_t seed)
    : callback_(callback), start_hex_(util::seed(seed))
{
    terminal_state_ = callback_(input{input_type::other, {}});
}

ftxui::Component user_interface::create()
{
    callback_(input{input_type::other, {}});
    return ftxui::CatchEvent(
        ftxui::Renderer([this]() {
            std::uint64_t start = start_hex_;

            auto attempts_window = [&]() {
                auto attempts = terminal_state_.attempts_left;
                std::string attempts_text = std::to_string(attempts) + " ATTEMPT(S) LEFT : ";

                ftxui::Elements attempt_elements;
                for (std::size_t i = 0; i < attempts; ++i)
                {
                    attempt_elements.push_back(ftxui::text(" ") | ftxui::inverted);
                    attempt_elements.push_back(ftxui::text(" "));
                }

                std::string warning = attempts <= 1 ? "!!! WARNING: LOCKOUT IMMINENT !!!" : "ENTER PASSWORD NOW";

                return ftxui::vbox({
                           ftxui::text("ROBCO INDUSTRIES (TM) TERMLINK PROTOCOL"),
                           ftxui::text(warning),
                           ftxui::text(""),
                           ftxui::hbox({ftxui::text(attempts_text), ftxui::hbox(attempt_elements)}),
                           ftxui::text(""),
                       }) |
                       ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, terminal_layout::attempts_window::height);
            }();

            auto hex_window_1 = ftxui::vbox([&]() {
                std::vector<ftxui::Element> hexes;
                start &= 0xFFFF;
                for (int i = 0; i < terminal_layout::interaction_window::hex_window_1::height; ++i)
                {
                    std::stringstream stream;
                    stream << "0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex << start;
                    hexes.push_back(ftxui::text(stream.str()));
                    start += terminal_layout::interaction_window::interactive_window_1::width;
                }
                return hexes;
            }());

            auto hex_window_2 = ftxui::vbox([&]() {
                std::vector<ftxui::Element> hexes;
                start &= 0xFFFF;
                for (int i = 0; i < terminal_layout::interaction_window::hex_window_2::height; ++i)
                {
                    std::stringstream stream;
                    stream << "0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex << start;
                    hexes.push_back(ftxui::text(stream.str()));
                    start += terminal_layout::interaction_window::interactive_window_2::width;
                }
                return hexes;
            }());

            auto interactive_window_1 = ftxui::vbox([&]() {
                std::vector<ftxui::Element> lines;
                std::string_view s = terminal_state_.term_chars;
                std::optional<highlight> hl = terminal_state_.highlighted;

                for (std::size_t i = 0; i < terminal_layout::interaction_window::interactive_window_1::height; ++i)
                {
                    std::vector<ftxui::Element> line;
                    for (std::size_t j = i * terminal_layout::interaction_window::interactive_window_1::width;
                         j < (i + 1) * terminal_layout::interaction_window::interactive_window_1::width; ++j)
                    {
                        if (hl.has_value() && (hl->begin <= j && j < hl->end))
                        {
                            line.push_back(ftxui::text(std::string{s[j]}) | ftxui::inverted);
                        }
                        else
                        {
                            line.push_back(ftxui::text(std::string{s[j]}));
                        }
                    }
                    lines.push_back(ftxui::hbox(line));
                }
                return lines;
            }());

            auto interactive_window_2 = ftxui::vbox([&]() {
                std::vector<ftxui::Element> lines;
                std::string_view s = terminal_state_.term_chars;
                std::optional<highlight> hl = terminal_state_.highlighted;

                for (std::size_t i = terminal_layout::interaction_window::interactive_window_2::height;
                     i < 2 * terminal_layout::interaction_window::interactive_window_2::height; ++i)
                {
                    std::vector<ftxui::Element> line;
                    for (std::size_t j = i * terminal_layout::interaction_window::interactive_window_2::width;
                         j < (i + 1) * terminal_layout::interaction_window::interactive_window_2::width; ++j)
                    {
                        if (hl.has_value() && (hl->begin <= j && j < hl->end))
                        {
                            line.push_back(ftxui::text(std::string{s[j]}) | ftxui::inverted);
                        }
                        else
                        {
                            line.push_back(ftxui::text(std::string{s[j]}));
                        }
                    }
                    lines.push_back(ftxui::hbox(line));
                }
                return lines;
            }());

            auto log_window = ftxui::vbox({
                ftxui::filler(),
                ftxui::vbox(log_),
                ftxui::text(""),
                ftxui::text(">" + last_loocked_),
            });

            auto interaction_window =
                ftxui::hbox({
                    hex_window_1 | ftxui::size(ftxui::WIDTH, ftxui::EQUAL,
                                               terminal_layout::interaction_window::hex_window_1::width),
                    ftxui::text(" "),
                    interactive_window_1 |
                        ftxui::size(ftxui::WIDTH, ftxui::EQUAL,
                                    terminal_layout::interaction_window::interactive_window_1::width),
                    ftxui::text(" "),
                    hex_window_2 | ftxui::size(ftxui::WIDTH, ftxui::EQUAL,
                                               terminal_layout::interaction_window::hex_window_2::width),
                    ftxui::text(" "),
                    interactive_window_2 |
                        ftxui::size(ftxui::WIDTH, ftxui::EQUAL,
                                    terminal_layout::interaction_window::interactive_window_2::width),
                    ftxui::text(" "),
                    log_window |
                        ftxui::size(ftxui::WIDTH, ftxui::EQUAL, terminal_layout::interaction_window::log_window::width),
                }) |
                ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, terminal_layout::interaction_window::height);

            return ftxui::vbox({
                       attempts_window,
                       interaction_window,
                   }) |
                   ftxui::size(ftxui::WIDTH, ftxui::EQUAL, terminal_layout::width) |
                   ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, terminal_layout::height) | ftxui::border;
        }),
        [this](ftxui::Event event) {
            if (event == ftxui::Event::Custom) // custom event - game over
            {
                callback_(input{input_type::other, {}});
                return true;
            }
            if (event.is_mouse()) // mouse did something
            {
                auto m_x = event.mouse().x;
                auto m_y = event.mouse().y;

                if (event.mouse().button == ftxui::Mouse::Left &&
                    event.mouse().motion == ftxui::Mouse::Released) // clicked left button
                {
                    terminal_state_ = callback_(input{input_type::click, screen_coords{m_x, m_y}});
                    update_internals_();
                }
                else
                {
                    terminal_state_ = callback_(input{input_type::look, screen_coords{m_x, m_y}});
                    update_internals_();
                }
                return true;
            }
            return false;
        });
}

void user_interface::update_internals_()
{
    if (terminal_state_.click_res.has_value()) // click happened
    {
        switch (terminal_state_.click_res->state)
        {
        case yafth::click_result::error:
            break;
        case yafth::click_result::dud_removed:
            log_.push_back(ftxui::text(">" + last_loocked_));
            log_.push_back(ftxui::text(">Dud Removed."));
            last_loocked_ = "";
            break;
        case yafth::click_result::allowance_replenished:
            log_.push_back(ftxui::text(">" + last_loocked_));
            log_.push_back(ftxui::text(">Allowance"));
            log_.push_back(ftxui::text(">Replenished"));
            last_loocked_ = "";
            break;
        case yafth::click_result::entry_denied:
            log_.push_back(ftxui::text(">" + last_loocked_));
            log_.push_back(ftxui::text(">Entry Denied"));
            log_.push_back(ftxui::text(">" + std::to_string(terminal_state_.click_res->match->of) + "/" +
                                       std::to_string(terminal_state_.click_res->match->from) + " correct."));
            last_loocked_ = "";
            break;
        case yafth::click_result::lockout_in_progress:
            log_.push_back(ftxui::text(">" + last_loocked_));
            log_.push_back(ftxui::text(">Entry Denied"));
            log_.push_back(ftxui::text(">" + std::to_string(terminal_state_.click_res->match->of) + "/" +
                                       std::to_string(terminal_state_.click_res->match->from) + " correct."));
            log_.push_back(ftxui::text(">Lockout in"));
            log_.push_back(ftxui::text(">progress."));
            last_loocked_ = "";
            break;
        case yafth::click_result::exact_match:
            log_.push_back(ftxui::text(">" + last_loocked_));
            log_.push_back(ftxui::text(">Exact match!"));
            log_.push_back(ftxui::text(">Please wait"));
            log_.push_back(ftxui::text(">while system"));
            log_.push_back(ftxui::text(">is accesed."));
            last_loocked_ = "";
            break;
        default:
            break;
        }

        if (log_.size() > terminal_layout::interaction_window::log_window::height - 2)
        {
            std::size_t to_erase = log_.size() - terminal_layout::interaction_window::log_window::height + 2;
            log_.erase(log_.begin(), log_.begin() + to_erase);
        }
    }
    else // look happened
    {
        if (terminal_state_.highlighted.has_value())
        {
            last_loocked_ = terminal_state_.term_chars.substr(terminal_state_.highlighted->begin,
                                                              terminal_state_.highlighted->end -
                                                                  terminal_state_.highlighted->begin);
        }
        else
        {
            last_loocked_ = "";
        }
    }
}
} // namespace yafth::ui