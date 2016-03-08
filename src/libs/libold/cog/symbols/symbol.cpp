#include "symbol.hpp"

gorc::cog::symbols::symbol::symbol(symbol_type type, const std::string& name,
    const vm::value& defaultvalue, bool local, const std::string& desc,
    int mask, int linkid, bool nolink)
    : type(type), name(name), default_value(defaultvalue), local(local),
    desc(desc), mask(mask), link_id(linkid), no_link(nolink)
{
    return;
}
