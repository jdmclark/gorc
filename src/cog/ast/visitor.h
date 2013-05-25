#pragma once

#include "node.h"
#include "framework/diagnostics/report.h"

#define AVPROTO(x) virtual void Visit##x(AST::x&);
#define AVCONSUME(x) virtual void Visit##x(AST::x&) { }
#define AVDEFAULTACTION(x) void Gorc::Cog::AST::Visitor::Visit##x(x& e) { DefaultAction(#x, e); }

#define ASTCLASS_SYMBOL_FIELD(x) \
	x(StringFragmentField) \
	x(IntegerField) \
	x(FloatField)

#define ASTCLASS_SYMBOL_EXTENSION(x) \
	x(BareExtension) \
	x(ValuedExtension)

#define ASTCLASS_SYMBOL(x) \
	x(Symbol) \
	x(ValuedSymbol)

#define ASTCLASS_EXPRESSION(x) \
	x(StringLiteralExpression) \
	x(IntegerLiteralExpression) \
	x(FloatLiteralExpression) \
	x(VectorLiteralExpression) \
	x(IdentifierExpression) \
	x(SubscriptExpression) \
	x(MethodCallExpression) \
	x(UnaryExpression) \
	x(InfixExpression) \
	x(AssignmentExpression) \
	x(CommaExpression) \
	x(ForOptionalExpression) \
	x(ForExpression)

#define ASTCLASS_FINAL_EXPRESSION(x) \
	x(ConstantValueExpression) \
	x(IdentifierExpression) \
	x(SubscriptExpression) \
	x(MethodCallExpression) \
	x(UnaryExpression) \
	x(InfixExpression) \
	x(AssignmentExpression) \
	x(CommaExpression) \
	x(ForOptionalExpression) \
	x(ForExpression)

#define ASTCLASS_STATEMENT(x) \
	x(CompoundStatement) \
	x(EmptyStatement) \
	x(ExpressionStatement) \
	x(BreakStatement) \
	x(ReturnStatement) \
	x(CallStatement) \
	x(IfStatement) \
	x(IfElseStatement) \
	x(WhileStatement) \
	x(DoStatement) \
	x(ForStatement) \
	x(LabeledStatement)

#define ASTCLASS_TRANSLATION_UNIT(x) \
	x(TranslationUnit)

#define ASTCLASS_ALL(x) \
	ASTCLASS_SYMBOL_FIELD(x) \
	ASTCLASS_SYMBOL_EXTENSION(x) \
	ASTCLASS_SYMBOL(x) \
	ASTCLASS_EXPRESSION(x) \
	ASTCLASS_STATEMENT(x) \
	ASTCLASS_TRANSLATION_UNIT(x) \
	x(ConstantValueExpression)

namespace Gorc {
namespace Cog {
namespace AST {

class Visitor
{
protected:
	const std::string VisitorName;
	Diagnostics::Report& Report;

	virtual void DefaultAction(const std::string& action, Node& n);

	Visitor(const std::string& name, Diagnostics::Report& report);

public:
	virtual ~Visitor();

	ASTCLASS_ALL(AVPROTO)
};

}
}
}
