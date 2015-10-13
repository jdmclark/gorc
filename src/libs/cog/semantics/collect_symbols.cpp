#include "collect_symbols.hpp"
#include "log/log.hpp"
#include "symbol_field_visitor.hpp"
#include "symbol_extension_type.hpp"
#include "symbol_extension_visitor.hpp"
#include <unordered_set>

gorc::cog::collect_symbols_visitor::collect_symbols_visitor(script &output_script,
                                                            seen_label_set const &seen_labels)
    : output_script(output_script)
    , seen_labels(seen_labels)
{
    return;
}

gorc::cog::value gorc::cog::collect_symbols_visitor::get_value(ast::symbol &symbol,
                                                               value_type type)
{
    return maybe_if_else(symbol.value,
                         [&](ast::symbol_field *field) {
            return ast::visit(symbol_field_visitor(output_script, type), *field);
        },
                         [&] {
            switch(type) {
            case value_type::floating:
                return value(0.0f);

            case value_type::integer:
                return value(0);

            case value_type::vector:
                return value(vector<3>());

            default:
                return value();
            }
        });
}

void gorc::cog::collect_symbols_visitor::visit(ast::list_node<ast::symbol*> &symbols)
{
    for(auto &symbol : symbols.elements) {
        ast::visit(*this, symbol);
    }
}

void gorc::cog::collect_symbols_visitor::visit(ast::symbol &symbol)
{
    value_type type = get_type(symbol);
    value default_value = get_value(symbol, type);

    // Visit extensions
    symbol_extension_visitor w(type);
    ast::visit(w, symbol.extensions);

    int default_mask = 0;
    switch(type) {
    case value_type::sector:
        default_mask = 0x400;
        break;

    case value_type::surface:
        default_mask = 0x400;
        break;

    case value_type::thing:
        default_mask = 0x404;
        break;

    default:
        default_mask = 0xFFFFFFFF;
        break;
    }

    bool default_local = false;
    if(type == value_type::message) {
        default_local = true;
    }

    output_script.symbols.add_symbol(type,
                                     symbol.name->value,
                                     default_value,
                                     w.local || default_local,
                                     w.desc,
                                     maybe_if(w.mask, default_mask, [](int m) { return m; }),
                                     maybe_if(w.linkid, 0, [](int l) { return l; }),
                                     w.nolink);

    if(type == value_type::message && seen_labels.find(symbol.name->value) == seen_labels.end()) {
        LOG_WARNING(format("message '%s' was exported but label not defined") % symbol.name->value);
    }
}

namespace gorc {
    namespace cog {
        namespace {

            std::unordered_map<std::string, value_type> symbol_type_map {
                { "ai", value_type::ai },
                { "cog", value_type::cog },
                { "flex", value_type::floating },
                { "float", value_type::floating },
                { "int", value_type::integer },
                { "keyframe", value_type::keyframe },
                { "material", value_type::material },
                { "message", value_type::message },
                { "model", value_type::model },
                { "sector", value_type::sector },
                { "sound", value_type::sound },
                { "string", value_type::string },
                { "surface", value_type::surface },
                { "template", value_type::thing_template },
                { "thing", value_type::thing },
                { "vector", value_type::vector } };

        }
    }
}

gorc::cog::value_type gorc::cog::collect_symbols_visitor::get_type(ast::symbol &symbol)
{
    auto it = symbol_type_map.find(symbol.type->value);
    if(it == symbol_type_map.end()) {
        diagnostic_context dc(symbol.type->location);
        LOG_ERROR(format("unknown type '%s'") % symbol.type->value);
        return value_type::dynamic;
    }
    else {
        return it->second;
    }
}
