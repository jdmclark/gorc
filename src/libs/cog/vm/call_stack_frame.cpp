#include "call_stack_frame.hpp"

gorc::cog::call_stack_frame::call_stack_frame(script const &cog,
                                              size_t program_counter)
    : cog(cog)
    , program_counter(program_counter)
{
    return;
}
