#pragma once

namespace yafth::core
{

class renderer_interface
{
  public:
    virtual void display() = 0;
    virtual ~renderer_interface() = default;
};

} // namespace yafth::core