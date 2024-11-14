#include <yafth/ui/cout_renderer.h>
#include <yafth/core/types.h>

#include <iostream>

namespace yafth::ui
{
    void cout_renderer::display([[maybe_unused]]const state & current_state)
    {
        //std::cout << current_state.term_chars << '\n';
    }
} // namespace yafth::ui