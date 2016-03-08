#include "code_visitor.hpp"
#include "expression_visitor.hpp"
#include "nonval_expression_visitor.hpp"

#include <boost/format.hpp>

using namespace gorc::cog::ast;
using gorc::cog::stages::generate_code::code_visitor;

code_visitor::code_visitor(int& nextLabelNumber,
                           ir::printer& printer,
                           std::unordered_map<std::string, vm::value> const &ct,
                           const std::string breakLabel)
    : Printer(printer)
    , constant_table(ct)
    , nextLabelNumber(nextLabelNumber)
    , BreakLabel(breakLabel) {
    return;
}

std::string code_visitor::generate_label(const std::string& prefix) {
    int num = nextLabelNumber++;
    return boost::str(boost::format("%s%03d") % prefix % num);
}

void code_visitor::visit(compound_statement& s) {
    for(auto& stmt : *s.code) {
        stmt->accept(*this);
    }
}

void code_visitor::visit(empty_statement&) {
    return;
}

void code_visitor::visit(expression_statement& s) {
    non_valued_expression_visitor v(Printer, constant_table);
    s.expr->accept(v);
}

void code_visitor::visit(break_statement&) {
    Printer.jmp(BreakLabel);
}

void code_visitor::visit(return_statement&) {
    Printer.ret();
}

void code_visitor::visit(call_statement& s) {
    Printer.jal(s.label);
}

void code_visitor::visit(if_statement& s) {
    std::string endlabel = generate_label("B");

    expression_visitor v(Printer, constant_table);
    s.condition->accept(v);
    Printer.bf(endlabel);

    s.code->accept(*this);

    Printer.label(endlabel);
}

void code_visitor::visit(if_else_statement& s) {
    std::string elselabel = generate_label("B");
    std::string endlabel = generate_label("B");

    expression_visitor v(Printer, constant_table);
    s.condition->accept(v);

    Printer.bf(elselabel);

    s.code->accept(*this);
    Printer.jmp(endlabel);

    Printer.label(elselabel);
    s.else_code->accept(*this);

    Printer.label(endlabel);
}

void code_visitor::visit(while_statement& s) {
    std::string bodybeginlabel = generate_label("L");
    std::string breaklabel = generate_label("L");

    expression_visitor v(Printer, constant_table);
    s.condition->accept(v);
    Printer.bf(breaklabel);

    Printer.label(bodybeginlabel);

    code_visitor cv(nextLabelNumber, Printer, constant_table, breaklabel);
    s.code->accept(cv);

    s.condition->accept(v);
    Printer.bt(bodybeginlabel);

    Printer.label(breaklabel);
}

void code_visitor::visit(do_statement& s) {
    std::string bodybeginlabel = generate_label("L");
    std::string breaklabel = generate_label("L");

    Printer.label(bodybeginlabel);
    code_visitor cv(nextLabelNumber, Printer, constant_table, breaklabel);
    s.code->accept(cv);

    expression_visitor v(Printer, constant_table);
    s.condition->accept(v);
    Printer.bt(bodybeginlabel);

    Printer.label(breaklabel);
}

void code_visitor::visit(for_statement& s) {
    std::string bodybeginlabel = generate_label("L");
    std::string breaklabel = generate_label("L");

    non_valued_expression_visitor nv(Printer, constant_table);
    expression_visitor v(Printer, constant_table);

    s.initializer->accept(nv);

    s.condition->accept(v);
    Printer.bf(breaklabel);

    Printer.label(bodybeginlabel);
    code_visitor cv(nextLabelNumber, Printer, constant_table, breaklabel);
    s.code->accept(cv);

    s.incrementer->accept(nv);

    s.condition->accept(v);
    Printer.bt(bodybeginlabel);

    Printer.label(breaklabel);
}

void code_visitor::visit(labeled_statement& s) {
    Printer.label(s.label);
    s.code->accept(*this);
}
