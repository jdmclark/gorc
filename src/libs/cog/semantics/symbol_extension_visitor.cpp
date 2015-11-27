#include "symbol_extension_visitor.hpp"
#include "log/log.hpp"
#include "symbol_extension_type.hpp"
#include "symbol_extension_field_integer_visitor.hpp"
#include "symbol_extension_field_string_visitor.hpp"

using namespace gorc;
using namespace gorc::cog;

namespace {
    void redefinition(std::string const &name, bool defined)
    {
        if(defined) {
            LOG_WARNING(format("extension '%s' redefinition") % name);
        }
    }

    bool check_accepted_inner(value_type symbol_type,
                              symbol_extension_type ext_type)
    {
        switch(ext_type) {
        case symbol_extension_type::local:
            return symbol_type != value_type::message;

        case symbol_extension_type::linkid:
            return symbol_type == value_type::sector ||
                   symbol_type == value_type::surface ||
                   symbol_type == value_type::thing;

        case symbol_extension_type::nolink:
            return symbol_type == value_type::cog ||
                   symbol_type == value_type::sector ||
                   symbol_type == value_type::surface ||
                   symbol_type == value_type::thing;

        case symbol_extension_type::desc:
            return symbol_type != value_type::message;

        case symbol_extension_type::mask:
            return symbol_type == value_type::sector ||
                   symbol_type == value_type::surface ||
                   symbol_type == value_type::thing;
        }

        // Catch-all: above switch should handle all cases
        return true; // LCOV_EXCL_LINE
    }

    void check_accepted(value_type symbol_type,
                        symbol_extension_type ext_type,
                        std::string const &ext_name)
    {
        if(!check_accepted_inner(symbol_type, ext_type)) {
            LOG_WARNING(format("symbol does not accept extension '%s'") % ext_name);
        }
    }
}

gorc::cog::symbol_extension_visitor::symbol_extension_visitor(value_type type)
    : symbol_type(type)
{
    return;
}

void gorc::cog::symbol_extension_visitor::visit(ast_list_node<ast::symbol_extension*> &exts)
{
    for(auto &ext : exts.elements) {
        ast_visit(*this, *ext);
    }
}

void gorc::cog::symbol_extension_visitor::visit(ast::bare_extension &ext)
{
    auto const &name = ext.name->value;
    auto ext_type = ast_visit(get_symbol_extension_type_visitor(), ext.name);
    maybe_if(ext_type, [&](symbol_extension_type type) {
            check_accepted(symbol_type, type, name);

            switch(type) {
            case symbol_extension_type::linkid:
            case symbol_extension_type::mask:
            case symbol_extension_type::desc:
                LOG_ERROR(format("extension '%s' requires a value") % name);
                return;

            case symbol_extension_type::local:
                redefinition(name, local);
                local = true;
                break;

            case symbol_extension_type::nolink:
                redefinition(name, nolink);
                nolink = true;
                break;
            }
        });
}

void gorc::cog::symbol_extension_visitor::visit(ast::valued_extension &ext)
{
    auto const &name = ext.name->value;
    auto ext_type = ast_visit(get_symbol_extension_type_visitor(), ext.name);
    maybe_if(ext_type, [&](symbol_extension_type type) {
            check_accepted(symbol_type, type, name);

            switch(type) {
            case symbol_extension_type::local:
            case symbol_extension_type::nolink:
                LOG_ERROR(format("extension '%s' does not accept a value") % name);
                break;

            case symbol_extension_type::linkid:
                redefinition(name, linkid.has_value());
                linkid = ast_visit(symbol_extension_field_integer_visitor(name),
                                    *ext.value);
                break;

            case symbol_extension_type::mask:
                redefinition(name, mask.has_value());
                mask = ast_visit(symbol_extension_field_integer_visitor(name),
                                  *ext.value);
                break;

            case symbol_extension_type::desc:
                redefinition(name, !desc.empty());
                desc = ast_visit(symbol_extension_field_string_visitor(),
                                  *ext.value);
                break;
            }
        });
}
