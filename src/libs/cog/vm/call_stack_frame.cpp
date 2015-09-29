#include "call_stack_frame.hpp"

gorc::cog::call_stack_frame::call_stack_frame(script const &cog,
                                              heap &memory,
                                              size_t program_counter)
    : cog(cog)
    , memory(memory)
    , program_counter(program_counter)
{
    return;
}
