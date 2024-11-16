#include <yafth/app/game_controller.h>
#include <yafth/core/engine.h>
#include <yafth/ui/ftxui_input_handler.h>
#include <yafth/ui/ftxui_renderer.h>
#include <yafth/ui/ftxui_shared_state.h>

#include <memory>

int main()
{
    auto ftxui_shared = std::make_shared<yafth::ui::ftxui_shared_state>();
    auto input_handler = std::make_unique<yafth::ui::ftxui_input_handler>(ftxui_shared);
    auto renderer = std::make_unique<yafth::ui::ftxui_renderer>(ftxui_shared);

    auto engine = std::make_unique<yafth::core::engine>();

    yafth::app::game_controller controller(std::move(engine), std::move(renderer), std::move(input_handler));
    controller.run();

    return 0;
}
