#include "symbol_visitor.h"
#include "symbolfield_visitor.h"
#include "symbolext_visitor.h"
#include "base/diagnostics/helper.h"

#include <algorithm>
#include <boost/format.hpp>

using namespace gorc::cog::ast;
using gorc::cog::stages::semantic_analysis::symbol_visitor;

symbol_visitor::symbol_visitor(symbols::symbol_table& symbolTable,
    const std::unordered_set<std::string>& SeenLabels, diagnostics::report& report)
    : ast::visitor("Stage2::SymbolVisitor", report), symbol_table(symbolTable), SeenLabels(SeenLabels) {
    return;
}

void symbol_visitor::visit_symbol(symbol& symbol) {
    symbols::symbol_type type = get_type(symbol);

    vm::value defaultvalue;

    switch(type) {
    case symbols::symbol_type::flex:
    case symbols::symbol_type::floating:
        defaultvalue = 0.0f;
        break;

    case symbols::symbol_type::integer:
        defaultvalue = 0;
        break;

    case symbols::symbol_type::vector:
        defaultvalue = make_zero_vector<3, float>();
        break;

    default:
    case symbols::symbol_type::ai:
    case symbols::symbol_type::keyframe:
    case symbols::symbol_type::material:
    case symbols::symbol_type::model:
    case symbols::symbol_type::sound:
    case symbols::symbol_type::thing_template:
    case symbols::symbol_type::cog:
    case symbols::symbol_type::message:
    case symbols::symbol_type::sector:
    case symbols::symbol_type::surface:
    case symbols::symbol_type::string:
    case symbols::symbol_type::thing:
        break;
    }

    // Convert symbol name to lowercase for processing.
    std::transform(symbol.name.begin(), symbol.name.end(), symbol.name.begin(), tolower);

    // Visit extension
    symbol_extension_visitor w(type, report);
    for(auto& ext : *symbol.extensions) {
        ext->accept(w);
    }

    if(symbol_table.is_symbol_defined(symbol.name)) {
        diagnostics::helper::symbol_redefinition(report, visitor_name, symbol.name, symbol.location);
        symbol_table.replace_symbol(type, symbol.name, defaultvalue, w.local, w.desc, w.mask, w.linkid, w.nolink);
    }
    else {
        symbol_table.add_symbol(type, symbol.name, defaultvalue, w.local, w.desc, w.mask, w.linkid, w.nolink);

        if(type == symbols::symbol_type::message) {
            if(SeenLabels.find(symbol.name) == SeenLabels.end()) {
                diagnostics::helper::missing_export(report, visitor_name, symbol.name, symbol.location);
            }
        }
    }
}

void symbol_visitor::visit_valued_symbol(valued_symbol& symbol) {
    symbol_field_visitor v(report);
    symbol.value->accept(v);

    symbols::symbol_type type = get_type(symbol);

    vm::value defaultvalue(0);

    switch(type) {
    case symbols::symbol_type::flex:
    case symbols::symbol_type::floating:
        if(!v.is_numeric) {
            diagnostics::helper::type_mismatch(report, visitor_name, symbol.location);
        }
        else {
            defaultvalue = vm::value(v.float_value);
        }
        break;

    case symbols::symbol_type::integer:
        if(!v.is_numeric) {
            diagnostics::helper::type_mismatch(report, visitor_name, symbol.location);
        }
        else {
            defaultvalue = vm::value(v.int_value);
        }
        break;
    
    case symbols::symbol_type::ai:
    case symbols::symbol_type::keyframe:
    case symbols::symbol_type::material:
    case symbols::symbol_type::model:
    case symbols::symbol_type::sound:
    case symbols::symbol_type::thing_template:
        if(!v.is_str) {
            diagnostics::helper::type_mismatch(report, visitor_name, symbol.location);
        }
        else {
            defaultvalue = vm::value(symbol_table.add_string(v.str_value));
        }
        break;

    default:
    case symbols::symbol_type::cog:
    case symbols::symbol_type::message:
    case symbols::symbol_type::sector:
    case symbols::symbol_type::string:
    case symbols::symbol_type::surface:
    case symbols::symbol_type::thing:
    case symbols::symbol_type::vector:
        diagnostics::helper::illegal_assignment(report, visitor_name, symbol.location);
        break;
    }

    // Convert symbol name to lowercase for processing.
    std::transform(symbol.name.begin(), symbol.name.end(), symbol.name.begin(), tolower);

    // Visit extension
    symbol_extension_visitor w(type, report);
    for(auto& ext : *symbol.extensions) {
        ext->accept(w);
    }

    if(symbol_table.is_symbol_defined(symbol.name)) {
        diagnostics::helper::symbol_redefinition(report, visitor_name, symbol.name, symbol.location);
        symbol_table.replace_symbol(type, symbol.name, defaultvalue, w.local, w.desc, w.mask, w.linkid, w.nolink);
    }
    else {
        symbol_table.add_symbol(type, symbol.name, defaultvalue, w.local, w.desc, w.mask, w.linkid, w.nolink);
    }
}

gorc::cog::symbols::symbol_type symbol_visitor::get_type(symbol& symbol) {
    // Convert symbol type to lowercase for processing.
    std::transform(symbol.type.begin(), symbol.type.end(), symbol.type.begin(), tolower);

    if(symbol.type == "ai") {
        return symbols::symbol_type::ai;
    }
    else if(symbol.type == "cog") {
        return symbols::symbol_type::cog;
    }
    else if(symbol.type == "flex") {
        return symbols::symbol_type::flex;
    }
    else if(symbol.type == "float") {
        return symbols::symbol_type::floating;
    }
    else if(symbol.type == "int") {
        return symbols::symbol_type::integer;
    }
    else if(symbol.type == "keyframe") {
        return symbols::symbol_type::keyframe;
    }
    else if(symbol.type == "material") {
        return symbols::symbol_type::material;
    }
    else if(symbol.type == "message") {
        return symbols::symbol_type::message;
    }
    else if(symbol.type == "model") {
        return symbols::symbol_type::model;
    }
    else if(symbol.type == "sector") {
        return symbols::symbol_type::sector;
    }
    else if(symbol.type == "sound") {
        return symbols::symbol_type::sound;
    }
    else if(symbol.type == "string") {
        return symbols::symbol_type::string;
    }
    else if(symbol.type == "surface") {
        return symbols::symbol_type::surface;
    }
    else if(symbol.type == "template") {
        return symbols::symbol_type::thing_template;
    }
    else if(symbol.type == "thing") {
        return symbols::symbol_type::thing;
    }
    else if(symbol.type == "vector") {
        return symbols::symbol_type::vector;
    }
    else {
        diagnostics::helper::unknown_type(report, visitor_name, symbol.type, symbol.location);

        return symbols::symbol_type::integer;
    }
}
