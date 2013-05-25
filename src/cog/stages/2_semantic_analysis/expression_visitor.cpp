#include "expression_visitor.h"
#include "lval_visitor.h"
#include "framework/diagnostics/helper.h"
#include <algorithm>

using namespace Gorc::Cog::AST;
using Gorc::Cog::Stages::SemanticAnalysis::ExpressionVisitor;

ExpressionVisitor::ExpressionVisitor(Symbols::SymbolTable& symbolTable,
	const std::unordered_map<std::string, VM::Value>& constantTable,
	Verbs::VerbTable& verbTable, Diagnostics::Report& report)
	: AST::Visitor("Stage2::ExpressionVisitor", report), SymbolTable(symbolTable),
	ConstantTable(constantTable), VerbTable(verbTable),	ExpressionType(VM::Type::Void) {
	return;
}

void ExpressionVisitor::VisitStringLiteralExpression(StringLiteralExpression& e) {
	ExpressionType = VM::Type::String;
}

void ExpressionVisitor::VisitIntegerLiteralExpression(IntegerLiteralExpression& e) {
	ExpressionType = VM::Type::Integer;
}

void ExpressionVisitor::VisitFloatLiteralExpression(FloatLiteralExpression& e) {
	ExpressionType = VM::Type::Float;
}

void ExpressionVisitor::VisitVectorLiteralExpression(VectorLiteralExpression& e) {
	ExpressionType = VM::Type::Vector;
}

void ExpressionVisitor::VisitIdentifierExpression(IdentifierExpression& e) {
	// Convert identifier to lowercase for processing.
	std::transform(e.Identifier.begin(), e.Identifier.end(), e.Identifier.begin(), tolower);

	// Check constant table.
	auto it = ConstantTable.find(e.Identifier);
	if(it != ConstantTable.end()) {
		// Identifier represents a constant.
		ExpressionType = it->second.GetType();
	}
	else {
		// Identifier does not represent a constant.

		// Actual type is not known, but the result must be
		// an actual value. Fake the result.
		ExpressionType = VM::Type::Integer;

		if(!SymbolTable.IsSymbolDefined(e.Identifier)) {
			Diagnostics::Helper::UndefinedSymbol(Report, VisitorName, e.Identifier, e.Location);
			SymbolTable.AddSymbol(e.Identifier);
		}
	}
}

void ExpressionVisitor::VisitSubscriptExpression(SubscriptExpression& e) {
	// Convert base identifier to lowercase for processing.
	std::transform(e.Base.begin(), e.Base.end(), e.Base.begin(), tolower);

	if(!SymbolTable.IsSymbolDefined(e.Base)) {
		Diagnostics::Helper::UndefinedArrayBase(Report, VisitorName, e.Base, e.Location);
	}

	ExpressionVisitor subscript(SymbolTable, ConstantTable, VerbTable, Report);
	e.Index->Accept(subscript);

	if(subscript.ExpressionType == VM::Type::Void) {
		Diagnostics::Helper::IllegalVoidResult(Report, VisitorName, e.Index->Location);
	}

	// Actual type is not known, but the result must be
	// an actual value. Fake the result.
	ExpressionType = VM::Type::Integer;
}

void ExpressionVisitor::VisitMethodCallExpression(MethodCallExpression& e) {
	// Convert verb name to lowercase for processing.
	std::transform(e.Base.begin(), e.Base.end(), e.Base.begin(), tolower);

	if(!VerbTable.IsVerbDefined(e.Base)) {
		Diagnostics::Helper::UnknownVerb(Report, VisitorName, e.Base, e.Location);
		ExpressionType = VM::Type::Integer;
	}
	else {
		auto id = VerbTable.GetVerb(e.Base);
		size_t pcount = VerbTable.ParameterCount(id);
		ExpressionType = VerbTable.ReturnType(id);

		if(pcount != e.Arguments->size()) {
			Diagnostics::Helper::InvalidArgumentCount(Report, VisitorName, e.Base, pcount, e.Arguments->size(), e.Location);
		}
		else {
			for(auto& arg : *e.Arguments) {
				ExpressionVisitor av(SymbolTable, ConstantTable, VerbTable, Report);
				arg->Accept(av);

				if(av.ExpressionType == VM::Type::Void) {
					Diagnostics::Helper::IllegalVoidResult(Report, VisitorName, arg->Location);
				}
			}
		}
	}

	return;
}

void ExpressionVisitor::VisitUnaryExpression(UnaryExpression& e) {
	ExpressionVisitor v(SymbolTable, ConstantTable, VerbTable, Report);
	e.Base->Accept(v);

	if(v.ExpressionType == VM::Type::Void) {
		Diagnostics::Helper::IllegalVoidResult(Report, VisitorName, e.Base->Location);
		v.ExpressionType = VM::Type::Integer;
	}

	switch(e.Operator) {
	case UnaryOperator::Plus:
	case UnaryOperator::Minus:
		ExpressionType = v.ExpressionType;
		return;

	case UnaryOperator::Not:
		if(v.ExpressionType != VM::Type::Boolean) {
			Diagnostics::Helper::ResultNotBoolean(Report, VisitorName, e.Location);
		}
		
		ExpressionType = VM::Type::Boolean;
		return;
	}
}

void ExpressionVisitor::VisitInfixExpression(InfixExpression& e) {
	ExpressionVisitor left(SymbolTable, ConstantTable, VerbTable, Report);
	e.Left->Accept(left);

	ExpressionVisitor right(SymbolTable, ConstantTable, VerbTable, Report);
	e.Right->Accept(right);

	// Left and right must both have values.
	if(left.ExpressionType == VM::Type::Void) {
		Diagnostics::Helper::IllegalVoidResult(Report, VisitorName, e.Left->Location);
		left.ExpressionType = VM::Type::Integer;
	}

	if(right.ExpressionType == VM::Type::Void) {
		Diagnostics::Helper::IllegalVoidResult(Report, VisitorName, e.Right->Location);
		right.ExpressionType = VM::Type::Integer;
	}

	switch(e.Operator) {
	case InfixOperator::Addition:
	case InfixOperator::Subtraction:
	case InfixOperator::Multiplication:
	case InfixOperator::Division:
	case InfixOperator::Modulo:
		// Result must have a numeric type.
		// Fake the result.
		ExpressionType = VM::Type::Integer;
		break;

	case InfixOperator::Greater:
	case InfixOperator::GreaterEqual:
	case InfixOperator::Less:
	case InfixOperator::LessEqual:
	case InfixOperator::Equal:
	case InfixOperator::NotEqual:
		// Result must have a boolean type.
		ExpressionType = VM::Type::Boolean;
		break;

	case InfixOperator::And:
	case InfixOperator::Or:
	case InfixOperator::Xor:
		// Result must have an integer type.
		ExpressionType = VM::Type::Integer;
		break;

	case InfixOperator::LogicalAnd:
	case InfixOperator::LogicalOr:
		// Inputs must have boolean types.
		if(left.ExpressionType != VM::Type::Boolean) {
			Diagnostics::Helper::ResultNotBoolean(Report, VisitorName, e.Left->Location);
		}

		if(right.ExpressionType != VM::Type::Boolean) {
			Diagnostics::Helper::ResultNotBoolean(Report, VisitorName, e.Right->Location);
		}

		// Result must have a boolean type.
		ExpressionType = VM::Type::Boolean;
		break;
	}

	return;
}

void ExpressionVisitor::VisitAssignmentExpression(AssignmentExpression& e) {
	LValueVisitor target(SymbolTable, ConstantTable, VerbTable, Report);
	e.Target->Accept(target);

	ExpressionVisitor value(SymbolTable, ConstantTable, VerbTable, Report);
	e.Value->Accept(value);

	if(value.ExpressionType == VM::Type::Void) {
		Diagnostics::Helper::IllegalVoidResult(Report, VisitorName, e.Value->Location);
	}

	ExpressionType = value.ExpressionType;
}

void ExpressionVisitor::VisitCommaExpression(CommaExpression& e) {
	ExpressionVisitor left(SymbolTable, ConstantTable, VerbTable, Report);
	e.Left->Accept(left);

	ExpressionVisitor right(SymbolTable, ConstantTable, VerbTable, Report);
	e.Right->Accept(right);

	ExpressionType = right.ExpressionType;
}

void ExpressionVisitor::VisitForOptionalExpression(ForOptionalExpression& e) {
	ExpressionType = VM::Type::Boolean;
}

void ExpressionVisitor::VisitForExpression(ForExpression& e) {
	e.Condition->Accept(*this);
	return;
}
