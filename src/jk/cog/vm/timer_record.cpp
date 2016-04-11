#include "timer_record.hpp"

gorc::cog::timer_record::timer_record(time_delta const &duration,
                                      value param0,
                                      value param1)
    : duration(duration)
    , remaining(duration)
    , param0(param0)
    , param1(param1)
{
    return;
}

gorc::cog::timer_record::timer_record(deserialization_constructor_tag, binary_input_stream &bis)
    : duration(binary_deserialize<time_delta>(bis))
    , remaining(binary_deserialize<time_delta>(bis))
    , param0(binary_deserialize<value>(bis))
    , param1(binary_deserialize<value>(bis))
{
    return;
}

void gorc::cog::timer_record::binary_serialize_object(binary_output_stream &bos) const
{
    binary_serialize(bos, duration);
    binary_serialize(bos, remaining);
    binary_serialize(bos, param0);
    binary_serialize(bos, param1);
}
