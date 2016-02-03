#include "call_stack_frame.hpp"

gorc::cog::call_stack_frame::call_stack_frame(asset_ref<script> cog,
                                              heap &memory,
                                              size_t program_counter,
                                              value sender,
                                              value source,
                                              value param0,
                                              value param1,
                                              value param2,
                                              value param3)
    : cog(cog)
    , memory(memory)
    , program_counter(program_counter)
    , sender(sender)
    , source(source)
    , param0(param0)
    , param1(param1)
    , param2(param2)
    , param3(param3)
{
    return;
}
