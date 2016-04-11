#include "executor_linkage.hpp"

gorc::cog::executor_linkage::executor_linkage(flag_set<source_type> mask,
                                              cog_id instance_id,
                                              value sender_link_id)
    : mask(mask)
    , instance_id(instance_id)
    , sender_link_id(sender_link_id)
{
    return;
}

gorc::cog::executor_linkage::executor_linkage(deserialization_constructor_tag,
                                              binary_input_stream &is)
    : mask(binary_deserialize<flag_set<source_type>>(is))
    , instance_id(binary_deserialize<cog_id>(is))
    , sender_link_id(binary_deserialize<value>(is))
{
    return;
}

void gorc::cog::executor_linkage::binary_serialize_object(binary_output_stream &os) const
{
    binary_serialize(os, mask);
    binary_serialize(os, instance_id);
    binary_serialize(os, sender_link_id);
}
