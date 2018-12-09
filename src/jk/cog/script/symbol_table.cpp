#include "symbol_table.hpp"
#include "log/log.hpp"

gorc::cog::symbol_table::symbol_table()
{
    constexpr size_t num_named_global_symbols = 16;
    for(size_t i = 0; i < num_named_global_symbols; ++i) {
        auto gs_name = str(format("global%d") % i);
        global_symbols.emplace_back(i, gs_name);
        global_index.emplace(std::move(gs_name), i);
    }
}

size_t gorc::cog::symbol_table::internal_add_symbol(symbol &&sym)
{
    auto jt = global_index.find(sym.name);
    if(jt != global_index.end()) {
        LOG_WARNING(format("symbol '%s' shadows global symbol") % sym.name);
    }

    auto it = index.find(sym.name);
    if(it != index.end()) {
        LOG_WARNING(format("symbol '%s' redefinition") % sym.name);
    }

    size_t idx = symbols.size();
    index[sym.name] = idx;
    symbols.push_back(std::forward<symbol>(sym));

    return idx;
}

std::tuple<gorc::cog::symbol_scope, size_t>
    gorc::cog::symbol_table::get_symbol_index(std::string const &name)
{
    auto it = index.find(name);
    if(it != index.end()) {
        return std::make_tuple(symbol_scope::local_symbol, it->second);
    }

    auto jt = global_index.find(name);
    if(jt != global_index.end()) {
        return std::make_tuple(symbol_scope::global_symbol, jt->second);
    }

    LOG_WARNING(format("symbol '%s' used without being defined") % name);
    return std::make_tuple(symbol_scope::local_symbol,
                           internal_add_symbol(symbol(symbols.size(), name)));
}

std::tuple<gorc::cog::symbol_scope, gorc::cog::symbol const *>
    gorc::cog::symbol_table::get_symbol_by_index(symbol_scope scope, size_t idx) const
{
    switch(scope) {
    case symbol_scope::global_symbol:
        return std::make_tuple(scope, &global_symbols.at(idx));

    default:
    case symbol_scope::local_symbol:
        return std::make_tuple(scope, &symbols.at(idx));
    }
}

std::tuple<gorc::cog::symbol_scope, gorc::cog::symbol const *>
    gorc::cog::symbol_table::get_symbol(std::string const &name)
{
    auto si = get_symbol_index(name);
    return get_symbol_by_index(std::get<0>(si), std::get<1>(si));
}

gorc::maybe<std::tuple<gorc::cog::symbol_scope, size_t>>
    gorc::cog::symbol_table::get_existing_symbol_index(std::string const &name) const
{
    auto it = index.find(name);
    if(it != index.end()) {
        return std::make_tuple(symbol_scope::local_symbol, it->second);
    }

    auto jt = global_index.find(name);
    if(jt != global_index.end()) {
        return std::make_tuple(symbol_scope::global_symbol, jt->second);
    }

    return nothing;
}

gorc::maybe<std::tuple<gorc::cog::symbol_scope, gorc::cog::symbol const *>>
    gorc::cog::symbol_table::get_existing_symbol(std::string const &name) const
{
    auto esi = get_existing_symbol_index(name);
    if(esi.has_value()) {
        auto const &esi_ub = esi.get_value();
        return get_symbol_by_index(std::get<0>(esi_ub), std::get<1>(esi_ub));
    }

    return nothing;
}

size_t gorc::cog::symbol_table::size() const
{
    return symbols.size();
}
