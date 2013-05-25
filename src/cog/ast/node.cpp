#include "node.h"
#include "visitor.h"

Gorc::Cog::AST::Node::Node() {
	return;
}

Gorc::Cog::AST::Node::~Node() {
	return;
}

ASTVISITOR_ACCEPT_IMPL(StringFragmentField);
ASTVISITOR_ACCEPT_IMPL(IntegerField);
ASTVISITOR_ACCEPT_IMPL(FloatField);

ASTVISITOR_ACCEPT_IMPL(BareExtension);
ASTVISITOR_ACCEPT_IMPL(ValuedExtension);

ASTVISITOR_ACCEPT_IMPL(Symbol);
ASTVISITOR_ACCEPT_IMPL(ValuedSymbol);

ASTVISITOR_ACCEPT_IMPL(ConstantValueExpression);
ASTVISITOR_ACCEPT_IMPL(StringLiteralExpression);
ASTVISITOR_ACCEPT_IMPL(IntegerLiteralExpression);
ASTVISITOR_ACCEPT_IMPL(FloatLiteralExpression);
ASTVISITOR_ACCEPT_IMPL(VectorLiteralExpression);
ASTVISITOR_ACCEPT_IMPL(IdentifierExpression);
ASTVISITOR_ACCEPT_IMPL(SubscriptExpression);
ASTVISITOR_ACCEPT_IMPL(MethodCallExpression);
ASTVISITOR_ACCEPT_IMPL(UnaryExpression);
ASTVISITOR_ACCEPT_IMPL(InfixExpression);
ASTVISITOR_ACCEPT_IMPL(AssignmentExpression);
ASTVISITOR_ACCEPT_IMPL(CommaExpression);

ASTVISITOR_ACCEPT_IMPL(CompoundStatement);
ASTVISITOR_ACCEPT_IMPL(EmptyStatement);
ASTVISITOR_ACCEPT_IMPL(ExpressionStatement);
ASTVISITOR_ACCEPT_IMPL(BreakStatement);
ASTVISITOR_ACCEPT_IMPL(ReturnStatement);
ASTVISITOR_ACCEPT_IMPL(CallStatement);
ASTVISITOR_ACCEPT_IMPL(IfStatement);
ASTVISITOR_ACCEPT_IMPL(IfElseStatement);
ASTVISITOR_ACCEPT_IMPL(WhileStatement);
ASTVISITOR_ACCEPT_IMPL(DoStatement);
ASTVISITOR_ACCEPT_IMPL(ForOptionalExpression);
ASTVISITOR_ACCEPT_IMPL(ForExpression);
ASTVISITOR_ACCEPT_IMPL(ForStatement);
ASTVISITOR_ACCEPT_IMPL(LabeledStatement);

ASTVISITOR_ACCEPT_IMPL(TranslationUnit);
