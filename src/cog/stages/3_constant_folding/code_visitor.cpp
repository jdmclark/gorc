#include "code_visitor.h"
#include "expression_visitor.h"

using namespace gorc::cog::ast;
using gorc::cog::stages::constant_folding::code_visitor;

code_visitor::code_visitor(ast::factory& factory, symbols::symbol_table& symbolTable,
	const std::unordered_map<std::string, vm::value>& constantTable,
	diagnostics::report& report)
	: ast::visitor("Stage3::CodeVisitor", report), Factory(factory), symbol_table(symbolTable),
	ConstantTable(constantTable) {
	return;
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
	expression_visitor v(Factory, symbol_table, ConstantTable, report);
	s.expr->accept(v);
	s.expr = v.get_substitution();
}

void code_visitor::visit_break_statement(break_statement& s) {
	return;
}

void code_visitor::visit_return_statement(return_statement& s) {
	return;
}

void code_visitor::visit_call_statement(call_statement& s) {
	return;
}

void code_visitor::visit_if_statement(if_statement& s) {
	expression_visitor v(Factory, symbol_table, ConstantTable, report);
	s.condition->accept(v);
	s.condition = v.get_substitution();

	s.code->accept(*this);
}

void code_visitor::visit_if_else_statement(if_else_statement& s) {
	expression_visitor v(Factory, symbol_table, ConstantTable, report);
	s.condition->accept(v);
	s.condition = v.get_substitution();

	s.code->accept(*this);
	s.else_code->accept(*this);
}

void code_visitor::visit_while_statement(while_statement& s) {
	expression_visitor v(Factory, symbol_table, ConstantTable, report);
	s.condition->accept(v);
	s.condition = v.get_substitution();

	s.code->accept(*this);
}

void code_visitor::visit_do_statement(do_statement& s) {
	expression_visitor v(Factory, symbol_table, ConstantTable, report);
	s.condition->accept(v);
	s.condition = v.get_substitution();

	s.code->accept(*this);
}

void code_visitor::visit_for_statement(for_statement& s) {
	expression_visitor v(Factory, symbol_table, ConstantTable, report);

	s.condition->accept(v);
	s.incrementer->accept(v);
	s.initializer->accept(v);

	s.code->accept(*this);
}

void code_visitor::visit_labeled_statement(labeled_statement& s) {
	s.code->accept(*this);
}
