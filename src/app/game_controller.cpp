#include <yafth/app/game_controller.h>

#include <yafth/core/engine_interface.h>
#include <yafth/core/input_handler_interface.h>
#include <yafth/core/renderer_interface.h>
#include <yafth/core/types.h>

#include <memory>

namespace yafth::app
{
game_controller::game_controller(std::unique_ptr<core::engine_interface> eng_ptr,
                                 std::unique_ptr<core::renderer_interface> rend_ptr,
                                 std::unique_ptr<core::input_handler_interface> input_ptr)
    : eng_ptr_(std::move(eng_ptr))
    , rend_ptr_(std::move(rend_ptr))
    , input_ptr_(std::move(input_ptr))
{}

void game_controller::run()
{
    state start_state = eng_ptr_->process_input(input{ input_type::other, {} });
    rend_ptr_->initialize(start_state);
    while (true)
    {
        input current_input = input_ptr_->get_input();
        state current_state = eng_ptr_->process_input(current_input);
        rend_ptr_->display(current_state);
        if (current_state.click_res.has_value() &&
            (current_state.click_res->state == click_result::exact_match ||
             current_state.click_res->state == click_result::lockout_in_progress))
        {
            break;
        }
    }
}
} // namespace yafth::app