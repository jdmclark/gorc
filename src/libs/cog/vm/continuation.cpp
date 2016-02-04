#include "continuation.hpp"

gorc::cog::continuation::continuation(call_stack_frame &&frame)
{
    call_stack.push_back(std::forward<call_stack_frame>(frame));
}

gorc::cog::continuation::continuation(deserialization_constructor_tag, binary_input_stream &bis)
{
    binary_deserialize_range<call_stack_frame>(bis, std::back_inserter(call_stack));
    binary_deserialize_range<value>(bis, std::back_inserter(data_stack));
}

void gorc::cog::continuation::binary_serialize_object(binary_output_stream &bos) const
{
    binary_serialize_range(bos, call_stack);
    binary_serialize_range(bos, data_stack);
}

gorc::cog::call_stack_frame& gorc::cog::continuation::frame()
{
    return call_stack.back();
}
