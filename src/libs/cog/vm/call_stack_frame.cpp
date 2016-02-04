#include "call_stack_frame.hpp"

gorc::cog::call_stack_frame::call_stack_frame(size_t instance_id,
                                              size_t program_counter,
                                              value sender,
                                              value source,
                                              value param0,
                                              value param1,
                                              value param2,
                                              value param3)
    : instance_id(instance_id)
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

gorc::cog::call_stack_frame::call_stack_frame(deserialization_constructor_tag,
                                              binary_input_stream &bis)
    : instance_id(binary_deserialize<size_t>(bis))
    , program_counter(binary_deserialize<size_t>(bis))
    , sender(binary_deserialize<value>(bis))
    , source(binary_deserialize<value>(bis))
    , param0(binary_deserialize<value>(bis))
    , param1(binary_deserialize<value>(bis))
    , param2(binary_deserialize<value>(bis))
    , param3(binary_deserialize<value>(bis))
    , return_register(binary_deserialize<value>(bis))
    , save_return_register(binary_deserialize<bool>(bis))
    , push_return_register(binary_deserialize<bool>(bis))
{
    return;
}

void gorc::cog::call_stack_frame::binary_serialize_object(binary_output_stream &bos) const
{
    binary_serialize(bos, instance_id);
    binary_serialize(bos, program_counter);
    binary_serialize(bos, sender);
    binary_serialize(bos, source);
    binary_serialize(bos, param0);
    binary_serialize(bos, param1);
    binary_serialize(bos, param2);
    binary_serialize(bos, param3);
    binary_serialize(bos, return_register);
    binary_serialize(bos, save_return_register);
    binary_serialize(bos, push_return_register);
}
