#pragma once

#include "node.h"
#include <memory>

#define MAKE_NOLINES(x)							\
	x* val = new x;								\
	nodes.push_back(std::unique_ptr<x>(val));

#define MAKE( x )								\
	MAKE_NOLINES(x)								\
	val->Location = yyl;

namespace Gorc {
namespace Cog {
namespace AST {

class Factory {
private:
	std::vector<std::unique_ptr<Node>> nodes;

	template <typename T> class ValueStorageNode : public Node {
	public:
		ValueStorageNode() {
			return;
		}

		T Value;

		void Accept(Visitor& v) {
			return;
		}
	};

public:
	template <typename T> T* StoreValue(const T& value) {
		MAKE_NOLINES(ValueStorageNode<T>);
		val->Value = value;
		return &val->Value;
	}

	template <typename T> std::vector<T*>* MakeList(T* initial_value) {
		MAKE_NOLINES(ValueStorageNode<std::vector<T*>>);
		val->Value.push_back(initial_value);
		return &val->Value;
	}

	template <typename T> std::vector<T*>* MakeList() {
		MAKE_NOLINES(ValueStorageNode<std::vector<T*>>);
		return &val->Value;
	}

	StringFragmentField* MakeStringFragmentField(const char* text, const Diagnostics::ErrorLocation& yyl);
	IntegerField* MakeIntegerField(int value, const Diagnostics::ErrorLocation& yyl);
	FloatField* MakeFloatField(float value, const Diagnostics::ErrorLocation& yyl);

	BareExtension* MakeBareExtension(const char* name, const Diagnostics::ErrorLocation& yyl);
	ValuedExtension* MakeValuedExtension(const char* name, SymbolField* value, const Diagnostics::ErrorLocation& yyl);

	Symbol* MakeSymbol(const char* type, const char* name, std::vector<SymbolExtension*>* extensions, const Diagnostics::ErrorLocation& yyl);
	ValuedSymbol* MakeValuedSymbol(const char* type, const char* name, std::vector<SymbolExtension*>* extensions, SymbolField* value, const Diagnostics::ErrorLocation& yyl);

	ConstantValueExpression* MakeConstantValueExpression(const VM::Value& value, const Diagnostics::ErrorLocation& yyl);
	StringLiteralExpression* MakeStringLiteralExpression(const char* value, const Diagnostics::ErrorLocation& yyl);
	IntegerLiteralExpression* MakeIntegerLiteralExpression(int value, const Diagnostics::ErrorLocation& yyl);
	FloatLiteralExpression* MakeFloatLiteralExpression(float value, const Diagnostics::ErrorLocation& yyl);
	VectorLiteralExpression* MakeVectorLiteralExpression(float x, float y, float z, const Diagnostics::ErrorLocation& yyl);
	IdentifierExpression* MakeIdentifierExpression(const char* identifier, const Diagnostics::ErrorLocation& yyl);
	SubscriptExpression* MakeSubscriptExpression(const char* base, Expression* index, const Diagnostics::ErrorLocation& yyl);
	MethodCallExpression* MakeMethodCallExpression(const char* base, std::vector<Expression*>* arguments, const Diagnostics::ErrorLocation& yyl);
	UnaryExpression* MakeUnaryExpression(Expression* base, UnaryOperator op, const Diagnostics::ErrorLocation& yyl);
	InfixExpression* MakeInfixExpression(Expression* left, Expression* right, InfixOperator op, const Diagnostics::ErrorLocation& yyl);
	AssignmentExpression* MakeAssignmentExpression(Expression* target, Expression* value, const Diagnostics::ErrorLocation& yyl);
	CommaExpression* MakeCommaExpression(Expression* left, Expression* right, const Diagnostics::ErrorLocation& yyl);

	CompoundStatement* MakeCompoundStatement(std::vector<Statement*>* code, const Diagnostics::ErrorLocation& yyl);
	EmptyStatement* MakeEmptyStatement(const Diagnostics::ErrorLocation& yyl);
	ExpressionStatement* MakeExpressionStatement(Expression* expression, const Diagnostics::ErrorLocation& yyl);
	BreakStatement* MakeBreakStatement(const Diagnostics::ErrorLocation& yyl);
	ReturnStatement* MakeReturnStatement(const Diagnostics::ErrorLocation& yyl);
	CallStatement* MakeCallStatement(const char* label, const Diagnostics::ErrorLocation& yyl);
	IfStatement* MakeIfStatement(Expression* condition, Statement* code, const Diagnostics::ErrorLocation& yyl);
	IfElseStatement* MakeIfElseStatement(Expression* condition, Statement* code, Statement* elsecode, const Diagnostics::ErrorLocation& yyl);
	WhileStatement* MakeWhileStatement(Expression* condition, Statement* code, const Diagnostics::ErrorLocation& yyl);
	DoStatement* MakeDoStatement(Statement* code, Expression* condition, const Diagnostics::ErrorLocation& yyl);
	ForOptionalExpression* MakeForOptionalExpression(const Diagnostics::ErrorLocation& yyl);
	ForExpression* MakeForExpression(Expression* condition, const Diagnostics::ErrorLocation& yyl);
	ForStatement* MakeForStatement(ForOptionalExpression* initializer, ForOptionalExpression* condition, ForOptionalExpression* incrementer, Statement* code, const Diagnostics::ErrorLocation& yyl);
	LabeledStatement* MakeLabeledStatement(const char* label, Statement* code, const Diagnostics::ErrorLocation& yyl);

	TranslationUnit* MakeTranslationUnit(std::vector<Symbol*>* symbols, std::vector<Statement*>* code, const Diagnostics::ErrorLocation& yyl);
};
}
}
}
