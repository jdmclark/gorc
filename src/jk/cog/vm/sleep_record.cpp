#include "sleep_record.hpp"

gorc::cog::sleep_record::sleep_record(continuation &&cc,
                                      time_delta expiration_time)
    : cc(std::forward<continuation>(cc))
    , expiration_time(expiration_time)
{
    return;
}

gorc::cog::sleep_record::sleep_record(deserialization_constructor_tag, binary_input_stream &bis)
    : cc(deserialization_constructor, bis)
    , expiration_time(binary_deserialize<time_delta>(bis))
{
    return;
}

void gorc::cog::sleep_record::binary_serialize_object(binary_output_stream &bos) const
{
    binary_serialize(bos, cc);
    binary_serialize(bos, expiration_time);
}
