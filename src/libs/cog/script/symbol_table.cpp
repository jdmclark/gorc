#include "symbol_table.hpp"
#include "log/log.hpp"

size_t gorc::cog::symbol_table::internal_add_symbol(symbol &&sym)
{
    auto it = index.find(sym.name);
    if(it != index.end()) {
        LOG_WARNING(format("symbol '%s' redefinition") % sym.name);
    }

    size_t idx = symbols.size();
    index[sym.name] = idx;
    symbols.push_back(std::forward<symbol>(sym));

    return idx;
}

size_t gorc::cog::symbol_table::get_symbol_index(std::string const &name)
{
    auto it = index.find(name);
    if(it == index.end()) {
        LOG_WARNING(format("symbol '%s' used without being defined") % name);
        return internal_add_symbol(symbol(symbols.size(), name));
    }

    return it->second;
}

gorc::cog::symbol const& gorc::cog::symbol_table::get_symbol_by_index(size_t idx) const
{
    return symbols[idx];
}

gorc::cog::symbol const& gorc::cog::symbol_table::get_symbol(std::string const &name)
{
    return get_symbol_by_index(get_symbol_index(name));
}

size_t gorc::cog::symbol_table::size() const
{
    return symbols.size();
}
