#include "code_visitor.h"
#include "expression_visitor.h"
#include "nonval_expression_visitor.h"

#include <boost/format.hpp>

using namespace gorc::cog::ast;
using gorc::cog::stages::generate_code::code_visitor;

code_visitor::code_visitor(int& nextLabelNumber, ir::printer& printer,
	diagnostics::report& report, const std::string breakLabel)
	: ast::visitor("Stage4::CodeVisitor", report), Printer(printer),
	  nextLabelNumber(nextLabelNumber), BreakLabel(breakLabel) {
	return;
}

std::string code_visitor::generate_label(const std::string& prefix) {
	int num = nextLabelNumber++;
	return boost::str(boost::format("%s%03d") % prefix % num);
}

void code_visitor::visit_compound_statement(compound_statement& s) {
	for(auto& stmt : *s.code) {
		stmt->accept(*this);
	}
}

void code_visitor::visit_empty_statement(empty_statement& s) {
	return;
}

void code_visitor::visit_expression_statement(expression_statement& s) {
	non_valued_expression_visitor v(Printer, report);
	s.expression->accept(v);
}

void code_visitor::visit_break_statement(break_statement& s) {
	Printer.jmp(BreakLabel);
}

void code_visitor::visit_return_statement(return_statement& s) {
	Printer.ret();
}

void code_visitor::visit_call_statement(call_statement& s) {
	Printer.jal(s.label);
}

void code_visitor::visit_if_statement(if_statement& s) {
	std::string endlabel = generate_label("B");

	expression_visitor v(Printer, report);
	s.condition->accept(v);
	Printer.bf(endlabel);

	s.code->accept(*this);

	Printer.label(endlabel);
}

void code_visitor::visit_if_else_statement(if_else_statement& s) {
	std::string elselabel = generate_label("B");
	std::string endlabel = generate_label("B");

	expression_visitor v(Printer, report);
	s.condition->accept(v);

	Printer.bf(elselabel);

	s.code->accept(*this);
	Printer.jmp(endlabel);

	Printer.label(elselabel);
	s.else_code->accept(*this);

	Printer.label(endlabel);
}

void code_visitor::visit_while_statement(while_statement& s) {
	std::string bodybeginlabel = generate_label("L");
	std::string breaklabel = generate_label("L");

	expression_visitor v(Printer, report);
	s.condition->accept(v);
	Printer.bf(breaklabel);

	Printer.label(bodybeginlabel);

	code_visitor cv(nextLabelNumber, Printer, report, breaklabel);
	s.code->accept(cv);

	s.condition->accept(v);
	Printer.bt(bodybeginlabel);

	Printer.label(breaklabel);
}

void code_visitor::visit_do_statement(do_statement& s) {
	std::string bodybeginlabel = generate_label("L");
	std::string breaklabel = generate_label("L");

	Printer.label(bodybeginlabel);
	code_visitor cv(nextLabelNumber, Printer, report, breaklabel);
	s.code->accept(cv);

	expression_visitor v(Printer, report);
	s.condition->accept(v);
	Printer.bt(bodybeginlabel);

	Printer.label(breaklabel);
}

void code_visitor::visit_for_statement(for_statement& s) {
	std::string bodybeginlabel = generate_label("L");
	std::string breaklabel = generate_label("L");

	non_valued_expression_visitor nv(Printer, report);
	expression_visitor v(Printer, report);

	s.initializer->accept(nv);

	s.condition->accept(v);
	Printer.bf(breaklabel);

	Printer.label(bodybeginlabel);
	code_visitor cv(nextLabelNumber, Printer, report, breaklabel);
	s.code->accept(cv);

	s.incrementer->accept(nv);

	s.condition->accept(v);
	Printer.bt(bodybeginlabel);

	Printer.label(breaklabel);
}

void code_visitor::visit_labeled_statement(labeled_statement& s) {
	Printer.label(s.label);
	s.code->accept(*this);
}
