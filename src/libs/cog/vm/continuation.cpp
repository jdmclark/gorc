#include "continuation.hpp"

gorc::cog::continuation::continuation(call_stack_frame &&frame)
{
    call_stack.push(std::forward<call_stack_frame>(frame));
}

gorc::cog::call_stack_frame& gorc::cog::continuation::frame()
{
    return call_stack.top();
}
