#include <yafth/app/game_controller.h>
#include <yafth/core/engine.h>
#include <yafth/ui/ftxui_input_handler.h>
#include <yafth/ui/ftxui_renderer.h>
#include <yafth/ui/ftxui_shared_state.h>
#include <yafth/util/args.h>

#include <memory>

// int main()
// {
//     auto ftxui_shared = std::make_shared<yafth::ui::ftxui_shared_state>();
//     auto engine = std::make_unique<yafth::core::engine>();
//     auto renderer = std::make_unique<yafth::ui::ftxui_renderer>(ftxui_shared);
//     auto input_handler = std::make_unique<yafth::ui::ftxui_input_handler>(ftxui_shared);

//     yafth::app::game_controller controller(std::move(engine), std::move(renderer), std::move(input_handler));
//     controller.run();

//     return 0;
// }

int main(int argc, char * argv[])
{
    yafth::util::args arguments;
    try
    {
        arguments = yafth::util::argparser::parse(argc, argv);
        std::cout << arguments.science_level << ' ' << static_cast<std::uint32_t>(arguments.lock) << ' ' << std::hex
                  << arguments.seed << '\n';
    } catch (const std::exception & ex)
    {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }

    auto ftxui_shared = std::make_shared<yafth::ui::ftxui_shared_state>(arguments.seed);
    auto engine =
        std::make_unique<yafth::core::engine>(arguments.lock, arguments.science_level, arguments.seed);
    auto renderer = std::make_unique<yafth::ui::ftxui_renderer>(ftxui_shared);
    auto input_handler = std::make_unique<yafth::ui::ftxui_input_handler>(ftxui_shared);

    yafth::app::game_controller controller(std::move(engine), std::move(renderer), std::move(input_handler));
    controller.run();

    return 0;
}
