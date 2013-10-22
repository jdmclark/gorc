#include "expression_visitor.h"

using namespace gorc::cog::ast;
using gorc::cog::stages::constant_folding::expression_visitor;

expression_visitor::expression_visitor(ast::factory& factory, symbols::symbol_table& symbolTable,
	const std::unordered_map<std::string, vm::value>& constantTable, diagnostics::report& report)
	: ast::visitor("Stage3::ExpressionVisitor", report), Factory(factory), symbol_table(symbolTable), ConstantTable(constantTable) {
	return;
}

void expression_visitor::visit_string_literal_expression(string_literal_expression& e) {
	visitedExpression = &e;
	isConstant = true;
	constantvalue = vm::value(symbol_table.add_string(e.value));
}

void expression_visitor::visit_integer_literal_expression(integer_literal_expression& e) {
	visitedExpression = &e;
	isConstant = true;
	constantvalue = vm::value(e.value);
}

void expression_visitor::visit_float_literal_expression(float_literal_expression& e) {
	visitedExpression = &e;
	isConstant = true;
	constantvalue = vm::value(e.value);
}

void expression_visitor::visit_vector_literal_expression(vector_literal_expression& e) {
	visitedExpression = &e;
	isConstant = true;
	constantvalue = vm::value(make_vector(e.x, e.y, e.z));
}

void expression_visitor::visit_identifier_expression(identifier_expression& e) {
	visitedExpression = &e;

	auto it = ConstantTable.find(e.identifier);
	if(it != ConstantTable.end()) {
		isConstant = true;
		constantvalue = it->second;
	}
	else {
		isConstant = false;
	}
}

void expression_visitor::visit_subscript_expression(subscript_expression& e) {
	visitedExpression = &e;
	isConstant = false;

	expression_visitor v(Factory, symbol_table, ConstantTable, report);
	e.index->accept(v);
	e.index = v.get_substitution();
}

void expression_visitor::visit_method_call_expression(method_call_expression& e) {
	visitedExpression = &e;
	isConstant = false;

	for(size_t i = 0; i < e.arguments->size(); ++i) {
		expression_visitor v(Factory, symbol_table, ConstantTable, report);
		(*e.arguments)[i]->accept(v);
		(*e.arguments)[i] = v.get_substitution();
	}
}

void expression_visitor::visit_unary_expression(unary_expression& e) {
	visitedExpression = &e;

	expression_visitor v(Factory, symbol_table, ConstantTable, report);
	e.base->accept(v);

	if(v.is_constant()) {
		isConstant = true;

		switch(e.op) {
		case unary_operator::plus:
			constantvalue = +v.get_constant_value();
			break;

		case unary_operator::minus:
			constantvalue = -v.get_constant_value();
			break;

		case unary_operator::logical_not:
			constantvalue = !v.get_constant_value();
			break;
		}
	}
	else {
		isConstant = false;
	}
}

void expression_visitor::visit_infix_expression(infix_expression& e) {
	visitedExpression = &e;

	expression_visitor v(Factory, symbol_table, ConstantTable, report);
	e.left->accept(v);

	expression_visitor w(Factory, symbol_table, ConstantTable, report);
	e.right->accept(w);

	if(v.is_constant() && w.is_constant()) {
		isConstant = true;

		switch(e.op) {
		case infix_operator::addition:
			constantvalue = v.get_constant_value() + w.get_constant_value();
			break;

		case infix_operator::subtraction:
			constantvalue = v.get_constant_value() - w.get_constant_value();
			break;

		case infix_operator::multiplication:
			constantvalue = v.get_constant_value() * w.get_constant_value();
			break;

		case infix_operator::division:
			constantvalue = v.get_constant_value() / w.get_constant_value();
			break;

		case infix_operator::modulo:
			constantvalue = v.get_constant_value() % w.get_constant_value();
			break;

		case infix_operator::greater:
			constantvalue = v.get_constant_value() > w.get_constant_value();
			break;

		case infix_operator::greater_equal:
			constantvalue = v.get_constant_value() >= w.get_constant_value();
			break;

		case infix_operator::less:
			constantvalue = v.get_constant_value() < w.get_constant_value();
			break;

		case infix_operator::less_equal:
			constantvalue = v.get_constant_value() <= w.get_constant_value();
			break;

		case infix_operator::equal:
			constantvalue = v.get_constant_value() == w.get_constant_value();
			break;

		case infix_operator::not_equal:
			constantvalue = v.get_constant_value() != w.get_constant_value();
			break;

		case infix_operator::bitwise_and:
			constantvalue = v.get_constant_value() & w.get_constant_value();
			break;

		case infix_operator::bitwise_or:
			constantvalue = v.get_constant_value() | w.get_constant_value();
			break;

		case infix_operator::bitwise_xor:
			constantvalue = v.get_constant_value() ^ w.get_constant_value();
			break;

		case infix_operator::logical_and:
			constantvalue = v.get_constant_value() && w.get_constant_value();
			break;

		case infix_operator::logical_or:
			constantvalue = v.get_constant_value() || w.get_constant_value();
			break;
		}
	}
	else {
		isConstant = false;

		e.left = v.get_substitution();
		e.right = w.get_substitution();
	}
}

void expression_visitor::visit_assignment_expression(assignment_expression& e) {
	visitedExpression = &e;
	isConstant = false;

	expression_visitor w(Factory, symbol_table, ConstantTable, report);
	e.target->accept(w);

	expression_visitor v(Factory, symbol_table, ConstantTable, report);
	e.value->accept(v);
	e.value = v.get_substitution();
}

void expression_visitor::visit_comma_expression(comma_expression& e) {
	visitedExpression = &e;
	isConstant = false;

	expression_visitor v(Factory, symbol_table, ConstantTable, report);
	
	e.left->accept(v);
	e.left = v.get_substitution();

	e.right->accept(v);
	e.right = v.get_substitution();
}

void expression_visitor::visit_for_optional_expression(for_optional_expression& e) {
	return;
}

void expression_visitor::visit_for_expression(for_expression& e) {
	expression_visitor v(Factory, symbol_table, ConstantTable, report);

	e.condition->accept(v);
	e.condition = v.get_substitution();
}
