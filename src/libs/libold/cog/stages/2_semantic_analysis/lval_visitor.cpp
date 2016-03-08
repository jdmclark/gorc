#include "lval_visitor.hpp"
#include "expression_visitor.hpp"
#include <algorithm>

using namespace gorc::cog::ast;
using gorc::cog::stages::semantic_analysis::lvalue_visitor;

lvalue_visitor::lvalue_visitor(symbols::symbol_table& symbolTable,
    const std::unordered_map<std::string, vm::value>& constantTable,
    verbs::verb_table& verbTable)
    : symbol_table(symbolTable),
    ConstantTable(constantTable), verb_table(verbTable) {
    return;
}

void lvalue_visitor::visit(ast::node &) {
    LOG_ERROR("assignment target is not an lvalue");
}

void lvalue_visitor::visit(identifier_expression& e) {
    // Convert identifier to lowercase for processing.
    std::transform(e.identifier.begin(), e.identifier.end(), e.identifier.begin(), tolower);

    if(!symbol_table.is_symbol_defined(e.identifier)) {
        LOG_WARNING(format("symbol '%s' used without being defined") % e.identifier);
        symbol_table.add_symbol(e.identifier);
    }
}

void lvalue_visitor::visit(subscript_expression& e) {
    // Convert base identifier to lowercase for processing.
    std::transform(e.base.begin(), e.base.end(), e.base.begin(), tolower);

    if(!symbol_table.is_symbol_defined(e.base)) {
        LOG_WARNING(format("symbol '%s' used without being defined") % e.base);
        symbol_table.add_symbol(e.base);
    }

    expression_visitor subscript(symbol_table, ConstantTable, verb_table);
    e.index->accept(subscript);

    if(subscript.expression_type == vm::type::nothing) {
        diagnostic_context dc(*e.index);
        LOG_ERROR("value expected, but expression has result type 'void'");
    }
}
