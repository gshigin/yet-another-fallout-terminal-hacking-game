#pragma once

#include <yafth/core/types.h>
#include <yafth/ui/ftxui_shared_state.h>
#include <yafth/util/random.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <string>
#include <vector>

namespace yafth::ui
{
class ftxui_shared_state
{
public:
    ftxui_shared_state()
        : screen_(ftxui::ScreenInteractive::FitComponent())
    {}
    ftxui_shared_state(std::uint64_t seed)
        : ftxui_shared_state()
    {
        start_hex_ = random::seed(seed);
    }

    void initialize(const state & initial_state);
    void display(const state & current_state);
    input get_input();

    ~ftxui_shared_state()
    {
        screen_.ExitLoopClosure()();
        if (loop_thread_.joinable())
        {
            loop_thread_.join();
        }
    }
private:
    void update_internals_();

    // as renderer
    std::thread loop_thread_;
    ftxui::ScreenInteractive screen_;
    std::uint64_t start_hex_ = 0;
    std::vector<ftxui::Element> log_ = {};
    state terminal_state_ = {};
    std::string last_loocked_ = "";
    ftxui::Component main_component_;

    // as input_handler
    input input_;
};

} // namespace yafth::ui
