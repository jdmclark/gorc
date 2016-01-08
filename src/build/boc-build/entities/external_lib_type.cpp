#include "external_lib_type.hpp"
#include "log/log.hpp"
#include "../engine/entity_serializer.hpp"
#include "../engine/entity_deserializer.hpp"
#include <unordered_map>

namespace {

    std::unordered_map<std::string, gorc::external_lib_type> lib_type_str_map {
        { "boost", gorc::external_lib_type::boost }
    };

}

gorc::external_lib_type gorc::to_external_lib_type(std::string const &str)
{
    auto it = lib_type_str_map.find(str);
    if(it == lib_type_str_map.end()) {
        LOG_FATAL(format("unknown external library type '%s'") % str);
    }

    return it->second;
}

gorc::ext_lib_set gorc::read_ext_lib_set(entity_input_stream &is)
{
    return is.read_range<ext_lib_set>([](ext_lib_set &rv, entity_input_stream &is) {
            rv.insert(static_cast<external_lib_type>(is.read_uint32()));
        });
}

void gorc::write_ext_lib_set(ext_lib_set const &els, entity_output_stream &os)
{
    os.write_range(els, [](external_lib_type t, entity_output_stream &os) {
            os.write_uint32(static_cast<uint32_t>(t));
        });
}
