#include "symbol_extension_type.hpp"
#include "log/log.hpp"
#include <unordered_map>

using namespace gorc;
using namespace gorc::cog;

namespace {
    std::unordered_map<std::string, symbol_extension_type> symbol_extension_type_map {
        { "local", symbol_extension_type::local },
        { "linkid", symbol_extension_type::linkid },
        { "nolink", symbol_extension_type::nolink },
        { "desc", symbol_extension_type::desc },
        { "mask", symbol_extension_type::mask } };
}

maybe<symbol_extension_type> get_symbol_extension_type_visitor::visit(ast::identifier &id) const
{
    auto it = symbol_extension_type_map.find(id.value);
    if(it == symbol_extension_type_map.end()) {
        LOG_ERROR(format("unknown extension '%s'") % id.value);
        return nothing;
    }

    return it->second;
}
