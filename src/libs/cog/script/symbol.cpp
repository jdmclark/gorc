#include "symbol.hpp"

gorc::cog::symbol::symbol(size_t sequence_number,
                          std::string const &name)
    : sequence_number(sequence_number)
    , type(value_type::dynamic)
    , name(name)
    , local(true)
    , mask(0)
    , link_id(0)
    , no_link(true)
{
    return;
}

gorc::cog::symbol::symbol(size_t sequence_number,
                          value_type type,
                          std::string const &name,
                          value default_value,
                          bool local,
                          std::string const &desc,
                          flag_set<source_type> mask,
                          int linkid,
                          bool nolink)
    : sequence_number(sequence_number)
    , type(type)
    , name(name)
    , default_value(default_value)
    , local(local)
    , desc(desc)
    , mask(mask)
    , link_id(linkid)
    , no_link(nolink)
{
    return;
}

bool gorc::cog::symbol::has_event_link() const
{
    if(no_link) {
        // No-link symbols never have event link
        return false;
    }

    switch(type) {
    case value_type::cog:
    case value_type::sector:
    case value_type::surface:
    case value_type::thing:
        return true;

    default:
        return false;
    }
}
