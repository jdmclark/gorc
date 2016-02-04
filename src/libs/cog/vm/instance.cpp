#include "instance.hpp"
#include "log/log.hpp"

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
    }
}

gorc::cog::instance::instance(deserialization_constructor_tag, binary_input_stream &bis)
    : cog(binary_deserialize<asset_ref<script>>(bis))
    , memory(deserialization_constructor, bis)
{
    return;
}

void gorc::cog::instance::binary_serialize_object(binary_output_stream &bos) const
{
    binary_serialize(bos, cog);
    binary_serialize(bos, memory);
}
