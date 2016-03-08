#include "expression_visitor.hpp"
#include "lval_expression_visitor.hpp"
#include "nonval_expression_visitor.hpp"

using namespace gorc::cog::ast;
using gorc::cog::stages::generate_code::expression_visitor;

expression_visitor::expression_visitor(ir::printer& printer,
                                       std::unordered_map<std::string, vm::value> const &ct)
    : Printer(printer)
    , constant_table(ct) {
    return;
}

void expression_visitor::visit(string_literal_expression &e) {
    vm::value val(e.stored_string);
    Printer.constant(val);
}

void expression_visitor::visit(integer_literal_expression &e) {
    vm::value val(e.value);
    Printer.constant(val);
}

void expression_visitor::visit(float_literal_expression &e) {
    vm::value val(e.value);
    Printer.constant(val);
}

void expression_visitor::visit(vector_literal_expression &e) {
    vm::value val(make_vector(e.x, e.y, e.z));
    Printer.constant(val);
}

void expression_visitor::visit(identifier_expression& e) {
    auto it = constant_table.find(e.identifier);
    if(it != constant_table.end()) {
        Printer.constant(it->second);
    }
    else {
        Printer.load(e.identifier);
    }
}

void expression_visitor::visit(subscript_expression& e) {
    e.index->accept(*this);
    Printer.loadi(e.base);
}

void expression_visitor::visit(method_call_expression& e) {
    for(auto& arg : *e.arguments) {
        arg->accept(*this);
    }

    Printer.callv(e.base);
}

void expression_visitor::visit(unary_expression& e) {
    e.base->accept(*this);

    switch(e.op) {
    case unary_operator::plus:
        break;

    case unary_operator::minus:
        Printer.neg();
        break;

    case unary_operator::logical_not:
        Printer.lnot();
        break;
    }
}

void expression_visitor::visit(infix_expression& e) {
    e.left->accept(*this);
    e.right->accept(*this);

    switch(e.op) {
    case infix_operator::addition:
        Printer.add();
        break;

    case infix_operator::subtraction:
        Printer.sub();
        break;

    case infix_operator::multiplication:
        Printer.mul();
        break;

    case infix_operator::division:
        Printer.div();
        break;

    case infix_operator::modulo:
        Printer.mod();
        break;

    case infix_operator::greater:
        Printer.cgt();
        break;

    case infix_operator::greater_equal:
        Printer.cgeq();
        break;

    case infix_operator::less:
        Printer.clt();
        break;

    case infix_operator::less_equal:
        Printer.cleq();
        break;

    case infix_operator::equal:
        Printer.ceq();
        break;

    case infix_operator::not_equal:
        Printer.cneq();
        break;

    case infix_operator::bitwise_and:
        Printer.band();
        break;

    case infix_operator::bitwise_or:
        Printer.bor();
        break;

    case infix_operator::bitwise_xor:
        Printer.bxor();
        break;

    case infix_operator::logical_and:
        Printer.land();
        break;

    case infix_operator::logical_or:
        Printer.lor();
        break;
    }
}

void expression_visitor::visit(assignment_expression& e) {
    e.value->accept(*this);

    // Copy to preserve value on stack after assignment
    Printer.copy();

    lvalue_expression_visitor v(Printer, constant_table);
    e.target->accept(v);
}

void expression_visitor::visit(comma_expression& e) {
    non_valued_expression_visitor v(Printer, constant_table);
    e.left->accept(v);
    e.right->accept(*this);
}

void expression_visitor::visit(for_empty_expression&) {
    // For optional expression always has value of 'true'
    Printer.constant(true);
}

void expression_visitor::visit(for_expression& e) {
    e.condition->accept(*this);
}
