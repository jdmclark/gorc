#include "instance.hpp"
#include "log/log.hpp"

gorc::cog::instance_linkage::instance_linkage(flag_set<source_type> mask,
                                              value sender_link_id)
    : mask(mask)
    , sender_link_id(sender_link_id)
{
    return;
}

gorc::cog::instance_linkage::instance_linkage(deserialization_constructor_tag,
                                              binary_input_stream &is)
    : mask(binary_deserialize<flag_set<source_type>>(is))
    , sender_link_id(binary_deserialize<value>(is))
{
    return;
}

void gorc::cog::instance_linkage::binary_serialize_object(binary_output_stream &os) const
{
    binary_serialize(os, mask);
    binary_serialize(os, sender_link_id);
}

gorc::cog::instance::instance(asset_ref<script> cog)
    : cog(cog)
{
    for(auto const &sym : cog->symbols) {
        memory[sym.sequence_number] = sym.default_value;
    }

    return;
}

gorc::cog::instance::instance(asset_ref<script> cog,
                              std::vector<value> const &values)
    : cog(cog)
{
    auto vt = values.begin();
    for(auto const &sym : cog->symbols) {
        if(sym.local) {
            memory[sym.sequence_number] = sym.default_value;
        }
        else if(vt == values.end()) {
            LOG_FATAL("not enough values to initialize heap");
        }
        else {
            memory[sym.sequence_number] = *vt;
            ++vt;
        }

        // Create linkage if required
        if(sym.has_event_link()) {
            linkages.emplace(memory[sym.sequence_number],
                             instance_linkage(sym.mask, sym.link_id));
        }
    }
}

gorc::cog::instance::instance(deserialization_constructor_tag, binary_input_stream &bis)
    : cog(binary_deserialize<asset_ref<script>>(bis))
    , memory(deserialization_constructor, bis)
{
    binary_deserialize_range(bis, std::inserter(linkages, linkages.end()), [](auto &bis) {
            auto obj = binary_deserialize<value>(bis);
            auto st = binary_deserialize<instance_linkage>(bis);
            return std::make_pair(obj, st);
        });

    return;
}

void gorc::cog::instance::binary_serialize_object(binary_output_stream &bos) const
{
    binary_serialize(bos, cog);
    binary_serialize(bos, memory);

    binary_serialize_range(bos, linkages, [](auto &bos, auto const &em) {
            binary_serialize(bos, em.first);
            binary_serialize(bos, em.second);
        });
}
