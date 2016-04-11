#include "pulse_record.hpp"

gorc::cog::pulse_record::pulse_record(time_delta const &duration)
    : duration(duration)
    , remaining(duration)
{
    return;
}

gorc::cog::pulse_record::pulse_record(deserialization_constructor_tag, binary_input_stream &bis)
    : duration(binary_deserialize<time_delta>(bis))
    , remaining(binary_deserialize<time_delta>(bis))
{
    return;
}

void gorc::cog::pulse_record::binary_serialize_object(binary_output_stream &bos) const
{
    binary_serialize(bos, duration);
    binary_serialize(bos, remaining);
}
