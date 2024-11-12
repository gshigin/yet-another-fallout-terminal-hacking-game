#include <yafth/app/game_controller.h>
#include <yafth/core/engine.h>
#include <yafth/ui/cin_input_handler.h>
#include <yafth/ui/cout_renderer.h>

#include <memory>

int main() 
{ 
    auto engine = std::make_unique<yafth::core::engine>();
    auto input_handler = std::make_unique<yafth::ui::cin_input_handler>();
    auto renderer = std::make_unique<yafth::ui::cout_renderer>();

    yafth::app::game_controller controller(std::move(engine), std::move(renderer), std::move(input_handler));
    controller.run();

    return 0; 
}
