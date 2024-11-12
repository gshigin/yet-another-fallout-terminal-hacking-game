#include <yafth/app/game_controller.h>

#include <yafth/core/types.h>
#include <yafth/core/engine.h>
#include <yafth/core/input_handler_interface.h>
#include <yafth/core/renderer_interface.h>

#include <memory>

namespace yafth::core
{
game_controller::game_controller(std::unique_ptr<engine> eng_ptr, std::unique_ptr<renderer_interface> rend_ptr,
                                 std::unique_ptr<input_handler_interface> input_ptr)
    : eng_ptr_(std::move(eng_ptr)), eng_ptr_(std::move(rend_ptr)), eng_ptr_(std::move(input_ptr))
{
}

void game_controller::run()
{
    core::state state = eng_ptr_->process_input(core::input{input_type::other, {}});
    rend_ptr_->display(state);
    while (true)
    {
        core::input input = input_ptr_->get_input();
        core::state state = eng_ptr_->process_input(input);
        rend_ptr_->display(state);
    }
}
} // namespace yafth::core