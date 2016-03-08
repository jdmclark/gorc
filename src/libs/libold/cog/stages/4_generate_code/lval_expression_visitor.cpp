#include "lval_expression_visitor.hpp"
#include "expression_visitor.hpp"

using namespace gorc::cog::ast;
using gorc::cog::stages::generate_code::lvalue_expression_visitor;

lvalue_expression_visitor::lvalue_expression_visitor(ir::printer& printer,
                                                     std::unordered_map<std::string, vm::value> const &ct)
    : Printer(printer)
    , constant_table(ct) {
    return;
}

void lvalue_expression_visitor::visit(node &) {
    LOG_ERROR("expression is not an lvalue");
}

void lvalue_expression_visitor::visit(identifier_expression& e) {
    Printer.store(e.identifier);
}

void lvalue_expression_visitor::visit(subscript_expression& e) {
    expression_visitor v(Printer, constant_table);
    e.index->accept(v);
    Printer.storei(e.base);
}
