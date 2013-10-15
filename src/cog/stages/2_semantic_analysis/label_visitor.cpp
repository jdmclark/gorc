#include "label_visitor.h"
#include "framework/diagnostics/helper.h"

#include <algorithm>

using namespace gorc::cog::ast;
using gorc::cog::stages::semantic_analysis::label_visitor;

label_visitor::label_visitor(std::unordered_set<std::string>& SeenLabels, diagnostics::report& report)
	: ast::visitor("Stage2::LabelVisitor", report), SeenLabels(SeenLabels) {
	return;
}

void label_visitor::visit_compound_statement(compound_statement& s) {
	for(auto& stmt : *s.code) {
		stmt->accept(*this);
	}
}

void label_visitor::visit_empty_statement(empty_statement& s) {
	return;
}

void label_visitor::visit_expression_statement(expression_statement& s) {
	return;
}

void label_visitor::visit_break_statement(break_statement& s) {
	return;
}

void label_visitor::visit_return_statement(return_statement& s) {
	return;
}

void label_visitor::visit_call_statement(call_statement& s) {
	return;
}

void label_visitor::visit_if_statement(if_statement& s) {
	s.code->accept(*this);
}

void label_visitor::visit_if_else_statement(if_else_statement& s) {
	s.code->accept(*this);
	s.else_code->accept(*this);
}

void label_visitor::visit_while_statement(while_statement& s) {
	s.code->accept(*this);
}

void label_visitor::visit_do_statement(do_statement& s) {
	s.code->accept(*this);
}

void label_visitor::visit_for_statement(for_statement& s) {
	s.code->accept(*this);
}

void label_visitor::visit_labeled_statement(labeled_statement& s) {
	// Convert label name to lowercase for processing
	std::transform(s.label.begin(), s.label.end(), s.label.begin(), tolower);

	if(SeenLabels.find(s.label) != SeenLabels.end()) {
		diagnostics::helper::label_redefinition(report, visitor_name, s.label, s.location);
	}
	else {
		SeenLabels.insert(s.label);
	}
}
