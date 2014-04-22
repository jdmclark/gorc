#include "symbolext_visitor.h"
#include <algorithm>
#include <boost/format.hpp>
#include "base/diagnostics/helper.h"
#include "symbolfield_visitor.h"
#include "cog/constants.h"

using namespace gorc::cog::ast;
using gorc::cog::stages::semantic_analysis::symbol_extension_visitor;

symbol_extension_visitor::symbol_extension_visitor(symbols::symbol_type type, diagnostics::report& report)
    : ast::visitor("Stage2::SymbolExtensionVisitor", report), symbol_type(type) {
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

bool symbol_extension_visitor::validate_extension(const std::string& name, text::location& location) {
    if(name == "local") {
        if(local) {
            diagnostics::helper::extension_redefinition(report, visitor_name, name, location);
        }

        switch(symbol_type) {
        case symbols::symbol_type::message:
            diagnostics::helper::illegal_extension(report, visitor_name, name, location);
            break;

        default:
            break;
        }
    }
    else if(name == "linkid") {
        if(linkid_defined) {
            diagnostics::helper::extension_redefinition(report, visitor_name, name, location);
        }

        switch(symbol_type) {
        case symbols::symbol_type::sector:
        case symbols::symbol_type::surface:
        case symbols::symbol_type::thing:
            break;

        default:
            diagnostics::helper::illegal_extension(report, visitor_name, name, location);
        }
    }
    else if(name == "nolink") {
        if(nolink) {
            diagnostics::helper::extension_redefinition(report, visitor_name, name, location);
        }

        switch(symbol_type) {
        case symbols::symbol_type::cog:
        case symbols::symbol_type::sector:
        case symbols::symbol_type::surface:
        case symbols::symbol_type::thing:
            break;

        default:
            diagnostics::helper::illegal_extension(report, visitor_name, name, location);
        }
    }
    else if(name == "desc") {
        if(!desc.empty()) {
            diagnostics::helper::extension_redefinition(report, visitor_name, name, location);
        }

        switch(symbol_type) {
        case symbols::symbol_type::message:
            diagnostics::helper::illegal_extension(report, visitor_name, name, location);
            break;

        default:
            break;
        }
    }
    else if(name == "mask") {
        if(mask_defined) {
            diagnostics::helper::extension_redefinition(report, visitor_name, name, location);
        }

        switch(symbol_type) {
        case symbols::symbol_type::sector:
        case symbols::symbol_type::surface:
        case symbols::symbol_type::thing:
            break;

        default:
            diagnostics::helper::illegal_extension(report, visitor_name, name, location);
        }
    }
    else {
        diagnostics::helper::unknown_extension(report, visitor_name, name, location);
        return false;
    }

    return true;
}

void symbol_extension_visitor::visit_bare_extension(bare_extension& ext) {
    std::transform(ext.name.begin(), ext.name.end(), ext.name.begin(), tolower);

    if(!validate_extension(ext.name, ext.location)) {
        return;
    }

    if(ext.name == "linkid" || ext.name == "desc" || ext.name == "mask") {
        diagnostics::helper::extension_value_missing(report, visitor_name, ext.name, ext.location);
        return;
    }
    else if(ext.name == "local") {
        local = true;
    }
    else if(ext.name == "nolink") {
        nolink = true;
    }
}

void symbol_extension_visitor::visit_valued_extension(valued_extension& ext) {
    std::transform(ext.name.begin(), ext.name.end(), ext.name.begin(), tolower);

    if(!validate_extension(ext.name, ext.location)) {
        return;
    }

    symbol_field_visitor v(report);
    ext.value->accept(v);

    if(ext.name == "local" || ext.name == "nolink") {
        diagnostics::helper::illegal_extension_assignment(report, visitor_name, ext.name, ext.location);
        return;
    }
    else if(ext.name == "linkid") {
        if(v.is_int) {
            linkid = v.int_value;
        }
        else {
            diagnostics::helper::extension_type_mismatch(report, visitor_name, ext.name, ext.location);
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
            diagnostics::helper::extension_type_mismatch(report, visitor_name, ext.name, ext.location);
        }
    }
    else if(ext.name == "mask") {
        if(v.is_int) {
            mask = v.int_value;
        }
        else {
            diagnostics::helper::extension_type_mismatch(report, visitor_name, ext.name, ext.location);
        }

        mask_defined = true;
    }
}
