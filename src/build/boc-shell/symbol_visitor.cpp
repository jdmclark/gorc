#include "symbol_visitor.hpp"
#include "log/log.hpp"
#include "symbols.hpp"

gorc::symbol_visitor::symbol_visitor(bool functions_allowed)
    : functions_allowed(functions_allowed)
{
    return;
}

void gorc::symbol_visitor::visit(command_statement &) const
{
    return;
}

void gorc::symbol_visitor::visit(assignment_statement &) const
{
    return;
}

void gorc::symbol_visitor::visit(var_declaration_statement &) const
{
    return;
}

void gorc::symbol_visitor::visit(func_declaration_statement &f) const
{
    if(!functions_allowed) {
        LOG_FATAL(format("function '%s' declaration is in disallowed context") %
                  f.name->value);
    }

    register_function(f);
}

void gorc::symbol_visitor::visit(if_statement &s) const
{
    ast_visit(symbol_visitor(false), *s.code);
}

void gorc::symbol_visitor::visit(if_else_statement &s) const
{
    ast_visit(symbol_visitor(false), *s.code);
    ast_visit(symbol_visitor(false), *s.elsecode);
}

void gorc::symbol_visitor::visit(return_statement &) const
{
    return;
}

void gorc::symbol_visitor::visit(call_statement &) const
{
    return;
}

void gorc::symbol_visitor::visit(ast_list_node<statement*> &stmt_seq) const
{
    for(auto &stmt : stmt_seq.elements) {
        ast_visit(*this, *stmt);
    }
}

void gorc::symbol_visitor::visit(compound_statement &stmt) const
{
    return ast_visit(*this, stmt.code);
}

void gorc::symbol_visitor::visit(translation_unit &tu) const
{
    return ast_visit(*this, tu.code);
}
