#include "code_visitor.h"
#include "expression_visitor.h"
#include "framework/diagnostics/helper.h"

#include <algorithm>

using namespace gorc::cog::ast;
using gorc::cog::stages::semantic_analysis::code_visitor;

code_visitor::code_visitor(symbols::symbol_table& symbolTable,
	const std::unordered_map<std::string, vm::value>& constantTable,
	verbs::verb_table& verbTable, const std::unordered_set<std::string>& SeenLabels,
	diagnostics::report& report, bool inside_loop, bool inside_block)
	: ast::visitor("Stage2::CodeVisitor", report), symbol_table(symbolTable),
	ConstantTable(constantTable), verb_table(verbTable),
	SeenLabels(SeenLabels), inside_loop(inside_loop), inside_block(inside_block) {
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
	expression_visitor v(symbol_table, ConstantTable, verb_table, report);
	s.expression->accept(v);
}

void code_visitor::visit_break_statement(break_statement& s) {
	if(!inside_loop) {
		diagnostics::helper::break_outside_loop(report, visitor_name, s.location);
	}
}

void code_visitor::visit_return_statement(return_statement& s) {
	return;
}

void code_visitor::visit_call_statement(call_statement& s) {
	// Convert label name to lowercase for processing
	std::transform(s.label.begin(), s.label.end(), s.label.begin(), tolower);

	if(SeenLabels.find(s.label) == SeenLabels.end()) {
		diagnostics::helper::call_to_unused_label(report, visitor_name, s.label, s.location);
	}
}

void code_visitor::visit_if_statement(if_statement& s) {
	expression_visitor cond(symbol_table, ConstantTable, verb_table, report);
	s.condition->accept(cond);

	if(cond.expression_type != vm::type::boolean) {
		diagnostics::helper::result_not_boolean(report, visitor_name, s.condition->location);
	}

	code_visitor v(symbol_table, ConstantTable, verb_table, SeenLabels, report, inside_loop, true);
	s.code->accept(v);
}

void code_visitor::visit_if_else_statement(if_else_statement& s) {
	expression_visitor cond(symbol_table, ConstantTable, verb_table, report);
	s.condition->accept(cond);

	if(cond.expression_type != vm::type::boolean) {
		diagnostics::helper::result_not_boolean(report, visitor_name, s.condition->location);
	}

	code_visitor first(symbol_table, ConstantTable, verb_table, SeenLabels, report, inside_loop, true);
	s.code->accept(first);

	code_visitor second(symbol_table, ConstantTable, verb_table, SeenLabels, report, inside_loop, true);
	s.else_code->accept(second);
}

void code_visitor::visit_while_statement(while_statement& s) {
	expression_visitor cond(symbol_table, ConstantTable, verb_table, report);
	s.condition->accept(cond);

	if(cond.expression_type != vm::type::boolean) {
		diagnostics::helper::result_not_boolean(report, visitor_name, s.condition->location);
	}

	code_visitor v(symbol_table, ConstantTable, verb_table, SeenLabels, report, true, true);
	s.code->accept(v);
}

void code_visitor::visit_do_statement(do_statement& s) {
	expression_visitor cond(symbol_table, ConstantTable, verb_table, report);
	s.condition->accept(cond);

	if(cond.expression_type != vm::type::boolean) {
		diagnostics::helper::result_not_boolean(report, visitor_name, s.condition->location);
	}

	code_visitor v(symbol_table, ConstantTable, verb_table, SeenLabels, report, true, true);
	s.code->accept(v);
}

void code_visitor::visit_for_statement(for_statement& s) {
	expression_visitor init(symbol_table, ConstantTable, verb_table, report);
	s.initializer->accept(init);

	expression_visitor cond(symbol_table, ConstantTable, verb_table, report);
	s.condition->accept(cond);

	if(cond.expression_type != vm::type::boolean) {
		diagnostics::helper::result_not_boolean(report, visitor_name, s.condition->location);
	}

	expression_visitor inc(symbol_table, ConstantTable, verb_table, report);
	s.incrementer->accept(inc);

	code_visitor v(symbol_table, ConstantTable, verb_table, SeenLabels, report, true, true);
	s.code->accept(v);
}

void code_visitor::visit_labeled_statement(labeled_statement& s) {
	if(inside_block) {
		diagnostics::helper::label_inside_block(report, visitor_name, s.label, s.location);
	}

	s.code->accept(*this);
}
