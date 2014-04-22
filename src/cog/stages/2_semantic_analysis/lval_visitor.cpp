#include "lval_visitor.h"
#include "expression_visitor.h"
#include "base/diagnostics/helper.h"
#include <algorithm>

using namespace gorc::cog::ast;
using gorc::cog::stages::semantic_analysis::lvalue_visitor;

lvalue_visitor::lvalue_visitor(symbols::symbol_table& symbolTable,
    const std::unordered_map<std::string, vm::value>& constantTable,
    verbs::verb_table& verbTable, diagnostics::report& report)
    : ast::visitor("Stage2::LvalueVisitor", report), symbol_table(symbolTable),
    ConstantTable(constantTable), verb_table(verbTable) {
    return;
}

void lvalue_visitor::default_action(const std::string&, node& n) {
    diagnostics::helper::assign_to_rvalue(report, visitor_name, n.location);
}

void lvalue_visitor::visit_identifier_expression(identifier_expression& e) {
    // Convert identifier to lowercase for processing.
    std::transform(e.identifier.begin(), e.identifier.end(), e.identifier.begin(), tolower);

    if(!symbol_table.is_symbol_defined(e.identifier)) {
        diagnostics::helper::undefined_symbol(report, visitor_name, e.identifier, e.location);
        symbol_table.add_symbol(e.identifier);
    }
}

void lvalue_visitor::visit_subscript_expression(subscript_expression& e) {
    // Convert base identifier to lowercase for processing.
    std::transform(e.base.begin(), e.base.end(), e.base.begin(), tolower);

    if(!symbol_table.is_symbol_defined(e.base)) {
        diagnostics::helper::undefined_symbol(report, visitor_name, e.base, e.location);
        symbol_table.add_symbol(e.base);
    }

    expression_visitor subscript(symbol_table, ConstantTable, verb_table, report);
    e.index->accept(subscript);

    if(subscript.expression_type == vm::type::nothing) {
        diagnostics::helper::illegal_void_result(report, visitor_name, e.index->location);
    }
}
