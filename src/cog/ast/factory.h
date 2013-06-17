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

	StringFragmentField* MakeStringFragmentField(const char* text, const Text::Location& yyl);
	IntegerField* MakeIntegerField(int value, const Text::Location& yyl);
	FloatField* MakeFloatField(float value, const Text::Location& yyl);

	BareExtension* MakeBareExtension(const char* name, const Text::Location& yyl);
	ValuedExtension* MakeValuedExtension(const char* name, SymbolField* value, const Text::Location& yyl);

	Symbol* MakeSymbol(const char* type, const char* name, std::vector<SymbolExtension*>* extensions, const Text::Location& yyl);
	ValuedSymbol* MakeValuedSymbol(const char* type, const char* name, std::vector<SymbolExtension*>* extensions, SymbolField* value, const Text::Location& yyl);

	ConstantValueExpression* MakeConstantValueExpression(const VM::Value& value, const Text::Location& yyl);
	StringLiteralExpression* MakeStringLiteralExpression(const char* value, const Text::Location& yyl);
	IntegerLiteralExpression* MakeIntegerLiteralExpression(int value, const Text::Location& yyl);
	FloatLiteralExpression* MakeFloatLiteralExpression(float value, const Text::Location& yyl);
	VectorLiteralExpression* MakeVectorLiteralExpression(float x, float y, float z, const Text::Location& yyl);
	IdentifierExpression* MakeIdentifierExpression(const char* identifier, const Text::Location& yyl);
	SubscriptExpression* MakeSubscriptExpression(const char* base, Expression* index, const Text::Location& yyl);
	MethodCallExpression* MakeMethodCallExpression(const char* base, std::vector<Expression*>* arguments, const Text::Location& yyl);
	UnaryExpression* MakeUnaryExpression(Expression* base, UnaryOperator op, const Text::Location& yyl);
	InfixExpression* MakeInfixExpression(Expression* left, Expression* right, InfixOperator op, const Text::Location& yyl);
	AssignmentExpression* MakeAssignmentExpression(Expression* target, Expression* value, const Text::Location& yyl);
	CommaExpression* MakeCommaExpression(Expression* left, Expression* right, const Text::Location& yyl);

	CompoundStatement* MakeCompoundStatement(std::vector<Statement*>* code, const Text::Location& yyl);
	EmptyStatement* MakeEmptyStatement(const Text::Location& yyl);
	ExpressionStatement* MakeExpressionStatement(Expression* expression, const Text::Location& yyl);
	BreakStatement* MakeBreakStatement(const Text::Location& yyl);
	ReturnStatement* MakeReturnStatement(const Text::Location& yyl);
	CallStatement* MakeCallStatement(const char* label, const Text::Location& yyl);
	IfStatement* MakeIfStatement(Expression* condition, Statement* code, const Text::Location& yyl);
	IfElseStatement* MakeIfElseStatement(Expression* condition, Statement* code, Statement* elsecode, const Text::Location& yyl);
	WhileStatement* MakeWhileStatement(Expression* condition, Statement* code, const Text::Location& yyl);
	DoStatement* MakeDoStatement(Statement* code, Expression* condition, const Text::Location& yyl);
	ForOptionalExpression* MakeForOptionalExpression(const Text::Location& yyl);
	ForExpression* MakeForExpression(Expression* condition, const Text::Location& yyl);
	ForStatement* MakeForStatement(ForOptionalExpression* initializer, ForOptionalExpression* condition, ForOptionalExpression* incrementer, Statement* code, const Text::Location& yyl);
	LabeledStatement* MakeLabeledStatement(const char* label, Statement* code, const Text::Location& yyl);

	TranslationUnit* MakeTranslationUnit(unsigned int flags, std::vector<Symbol*>* symbols, std::vector<Statement*>* code, const Text::Location& yyl);
};
}
}
}
