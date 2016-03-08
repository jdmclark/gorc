#include "symbolext_visitor.hpp"
#include <algorithm>
#include <boost/format.hpp>
#include "symbolfield_visitor.hpp"
#include "libold/cog/constants.hpp"

using namespace gorc::cog::ast;
using gorc::cog::stages::semantic_analysis::symbol_extension_visitor;

symbol_extension_visitor::symbol_extension_visitor(symbols::symbol_type type)
    : symbol_type(type) {
    // Set sensible default extensions.
    local = false;

    linkid_defined = false;
    linkid = cog::constants::default_link_id;

    nolink = false;

    mask_defined = false;

    switch(symbol_type) {
    default:
        mask = cog::constants::default_mask;
        break;

    case symbols::symbol_type::sector:
        mask = cog::constants::default_sector_mask;
        break;

    case symbols::symbol_type::surface:
        mask = cog::constants::default_surface_mask;
        break;

    case symbols::symbol_type::thing:
        mask = cog::constants::default_thing_mask;
        break;
    }

    return;
}

bool symbol_extension_visitor::validate_extension(const std::string& name) {
    if(name == "local") {
        if(local) {
            LOG_WARNING(format("extension '%s' redefinition") % name);
        }

        switch(symbol_type) {
        case symbols::symbol_type::message:
            LOG_WARNING(format("symbol does not accept extension '%s'") % name);
            break;

        default:
            break;
        }
    }
    else if(name == "linkid") {
        if(linkid_defined) {
            LOG_WARNING(format("extension '%s' redefinition") % name);
        }

        switch(symbol_type) {
        case symbols::symbol_type::sector:
        case symbols::symbol_type::surface:
        case symbols::symbol_type::thing:
            break;

        default:
            LOG_WARNING(format("symbol does not accept extension '%s'") % name);
        }
    }
    else if(name == "nolink") {
        if(nolink) {
            LOG_WARNING(format("extension '%s' redefinition") % name);
        }

        switch(symbol_type) {
        case symbols::symbol_type::cog:
        case symbols::symbol_type::sector:
        case symbols::symbol_type::surface:
        case symbols::symbol_type::thing:
            break;

        default:
            LOG_WARNING(format("symbol does not accept extension '%s'") % name);
        }
    }
    else if(name == "desc") {
        if(!desc.empty()) {
            LOG_WARNING(format("extension '%s' redefinition") % name);
        }

        switch(symbol_type) {
        case symbols::symbol_type::message:
            LOG_WARNING(format("symbol does not accept extension '%s'") % name);
            break;

        default:
            break;
        }
    }
    else if(name == "mask") {
        if(mask_defined) {
            LOG_WARNING(format("extension '%s' redefinition") % name);
        }

        switch(symbol_type) {
        case symbols::symbol_type::sector:
        case symbols::symbol_type::surface:
        case symbols::symbol_type::thing:
            break;

        default:
            LOG_WARNING(format("symbol does not accept extension '%s'") % name);
        }
    }
    else {
        LOG_WARNING(format("unknown extension '%s'") % name);
        return false;
    }

    return true;
}

void symbol_extension_visitor::visit(bare_extension& ext) {
    std::transform(ext.name.begin(), ext.name.end(), ext.name.begin(), tolower);

    if(!validate_extension(ext.name)) {
        return;
    }

    if(ext.name == "linkid" || ext.name == "desc" || ext.name == "mask") {
        LOG_ERROR(format("extension '%s' requires a value") % ext.name);
        return;
    }
    else if(ext.name == "local") {
        local = true;
    }
    else if(ext.name == "nolink") {
        nolink = true;
    }
}

void symbol_extension_visitor::visit(valued_extension& ext) {
    std::transform(ext.name.begin(), ext.name.end(), ext.name.begin(), tolower);

    if(!validate_extension(ext.name)) {
        return;
    }

    symbol_field_visitor v;
    ext.value->accept(v);

    if(ext.name == "local" || ext.name == "nolink") {
        LOG_ERROR(format("extension '%s' does not accept a value") % ext.name);
        return;
    }
    else if(ext.name == "linkid") {
        if(v.is_int) {
            linkid = v.int_value;
        }
        else {
            LOG_ERROR(format("extension '%s' value type mismatch") % ext.name);
        }

        linkid_defined = true;
    }
    else if(ext.name == "desc") {
        if(v.is_int) {
            desc = boost::str(boost::format("%d") % v.int_value);
        }
        else if(v.is_float) {
            desc = boost::str(boost::format("%f") % v.float_value);
        }
        else if(v.is_str) {
            desc = v.str_value;
        }
        else {
            LOG_ERROR(format("extension '%s' value type mismatch") % ext.name);
        }
    }
    else if(ext.name == "mask") {
        if(v.is_int) {
            mask = v.int_value;
        }
        else {
            LOG_ERROR(format("extension '%s' value type mismatch") % ext.name);
        }

        mask_defined = true;
    }
}
