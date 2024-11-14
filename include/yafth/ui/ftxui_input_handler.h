#pragma once

#include <yafth/core/input_handler_interface.h>
#include <yafth/core/types.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <iostream>

namespace yafth::ui
{

struct awaitable_input
{
    ftxui::ScreenInteractive & screen;
    input & last_input;
};

class ftxui_input_handler : public core::input_handler_interface
{
public:
    ftxui_input_handler()
        : screen_(ftxui::ScreenInteractive::FitComponent())
        , last_input_{ input_type::other, std::nullopt }
    {}
    ~ftxui_input_handler() override { screen_.ExitLoopClosure()(); }
    input get_input() override
    {
        auto input_component = ftxui::Renderer([this] { return ftxui::text(""); }) | ftxui::CatchEvent(
            [&](ftxui::Event event)
            {
                if (event.is_mouse()) // mouse did something
                {
                    auto m_x = event.mouse().x;
                    auto m_y = event.mouse().y;

                    if (event.mouse().button == ftxui::Mouse::Left &&
                        event.mouse().motion == ftxui::Mouse::Pressed) // clicked left button
                    {
                        last_input_ = input{
                            input_type::click, screen_coords{ m_x, m_y }
                        };
                    } else
                    {
                        last_input_ = input{
                            input_type::look, screen_coords{ m_x, m_y }
                        };
                    }
                    std::cout << m_x << " " << m_y << '\r';
                    std::cout.flush();
                    screen_.ExitLoopClosure()();
                    return true;
                }
                return false;
            });

        screen_.Loop(input_component);
        return last_input_;
    }
private:
    ftxui::ScreenInteractive screen_;
    input last_input_;
};
} // namespace yafth::ui