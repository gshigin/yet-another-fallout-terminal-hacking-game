#include <yafth/ui/cin_input_handler.h>
#include <yafth/core/types.h>

namespace yafth::ui
{
    input cin_input_handler::get_input()
    {
        return {input_type::other, {}};
    }
} // namespace yafth::ui