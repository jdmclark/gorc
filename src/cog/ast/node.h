#pragma once

#include "framework/diagnostics/errorlocation.h"
#include "cog/vm/value.h"
#include <string>
#include <vector>

#define ASTVISITOR_ACCEPT_ABSTRACT							\
	public:													\
	virtual void Accept(Gorc::Cog::AST::Visitor& v) = 0;	\
	private:

#define ASTVISITOR_ACCEPT									\
	public:													\
	virtual void Accept(Gorc::Cog::AST::Visitor& v);		\
	private:

#define ASTVISITOR_ACCEPT_IMPL( x )							\
	void Gorc::Cog::AST::x::Accept(Visitor& v) {			\
		v.Visit##x(*this);									\
		return;												\
	}

namespace Gorc {
namespace Cog {
namespace AST {

enum class UnaryOperator {
	Plus,
	Minus,
	Not
};

enum class InfixOperator {
	Addition,
	Subtraction,
	Multiplication,
	Division,
	Modulo,

	Greater,
	GreaterEqual,
	Less,
	LessEqual,
	Equal,
	NotEqual,

	And,
	Or,
	Xor,

	LogicalAnd,
	LogicalOr
};

class Visitor;

class Node {
	ASTVISITOR_ACCEPT_ABSTRACT
protected:
	Node();

public:
	virtual ~Node();

	Diagnostics::ErrorLocation Location;
};

/* Symbol section */

class SymbolField : public Node {
	ASTVISITOR_ACCEPT_ABSTRACT
public:
};

class StringFragmentField : public SymbolField {
	ASTVISITOR_ACCEPT
public:
	std::string Value;
};

class IntegerField : public SymbolField {
	ASTVISITOR_ACCEPT
public:
	int Value;
};

class FloatField : public SymbolField {
	ASTVISITOR_ACCEPT
public:
	float Value;
};

class SymbolExtension : public Node {
	ASTVISITOR_ACCEPT_ABSTRACT
public:
};

class BareExtension : public SymbolExtension {
	ASTVISITOR_ACCEPT
public:
	std::string Name;
};

class ValuedExtension : public BareExtension {
	ASTVISITOR_ACCEPT
public:
	SymbolField* Value;
};

class Symbol : public Node {
	ASTVISITOR_ACCEPT
public:
	std::string Type;
	std::string Name;
	std::vector<SymbolExtension*>* Extensions;
};

class ValuedSymbol : public Symbol {
	ASTVISITOR_ACCEPT
public:
	SymbolField* Value;
};

/* Expressions */
class Expression : public Node {
	ASTVISITOR_ACCEPT_ABSTRACT
};

class StringLiteralExpression : public Expression {
	ASTVISITOR_ACCEPT
public:
	std::string Value;
};

class IntegerLiteralExpression : public Expression {
	ASTVISITOR_ACCEPT
public:
	int Value;
};

class FloatLiteralExpression : public Expression {
	ASTVISITOR_ACCEPT
public:
	float Value;
};

class VectorLiteralExpression : public Expression {
	ASTVISITOR_ACCEPT
public:
	float X, Y, Z;
};

class ConstantValueExpression : public Expression {
	ASTVISITOR_ACCEPT
public:
	VM::Value Value;
};

class IdentifierExpression : public Expression {
	ASTVISITOR_ACCEPT
public:
	std::string Identifier;
};

class SubscriptExpression : public Expression {
	ASTVISITOR_ACCEPT
public:
	std::string Base;
	Expression* Index;
};

class MethodCallExpression : public Expression {
	ASTVISITOR_ACCEPT
public:
	std::string Base;
	std::vector<Expression*>* Arguments;
};

class UnaryExpression : public Expression {
	ASTVISITOR_ACCEPT
public:
	Expression* Base;
	UnaryOperator Operator;
};

class InfixExpression : public Expression {
	ASTVISITOR_ACCEPT
public:
	Expression* Left;
	Expression* Right;
	InfixOperator Operator;
};

class AssignmentExpression : public Expression {
	ASTVISITOR_ACCEPT
public:
	Expression* Target;
	Expression* Value;
};

class CommaExpression : public Expression {
	ASTVISITOR_ACCEPT
public:
	Expression* Left;
	Expression* Right;
};

/* Statements */
class Statement : public Node {
	ASTVISITOR_ACCEPT_ABSTRACT
};

class CompoundStatement : public Statement {
	ASTVISITOR_ACCEPT
public:
	std::vector<Statement*>* Code;
};

class EmptyStatement : public Statement {
	ASTVISITOR_ACCEPT
};

class ExpressionStatement : public Statement {
	ASTVISITOR_ACCEPT
public:
	Expression* Expression;
};

class BreakStatement : public Statement {
	ASTVISITOR_ACCEPT
};

class ReturnStatement : public Statement {
	ASTVISITOR_ACCEPT
};

class CallStatement : public Statement {
	ASTVISITOR_ACCEPT
public:
	std::string Label;
};

class IfStatement : public Statement {
	ASTVISITOR_ACCEPT
public:
	Expression* Condition;
	Statement* Code;
};

class IfElseStatement : public Statement {
	ASTVISITOR_ACCEPT
public:
	Expression* Condition;
	Statement* Code;
	Statement* ElseCode;
};

class WhileStatement : public Statement {
	ASTVISITOR_ACCEPT
public:
	Expression* Condition;
	Statement* Code;
};

class DoStatement : public Statement {
	ASTVISITOR_ACCEPT
public:
	Statement* Code;
	Expression* Condition;
};

class ForOptionalExpression : public Node {
	ASTVISITOR_ACCEPT
};

class ForExpression : public ForOptionalExpression {
	ASTVISITOR_ACCEPT
public:
	Expression* Condition;
};

class ForStatement : public Statement {
	ASTVISITOR_ACCEPT
public:
	ForOptionalExpression* Initializer;
	ForOptionalExpression* Condition;
	ForOptionalExpression* Incrementer;
	Statement* Code;
};

class LabeledStatement : public Statement {
	ASTVISITOR_ACCEPT
public:
	std::string Label;
	Statement* Code;
};

/* Translation unit */
class TranslationUnit : public Node {
	ASTVISITOR_ACCEPT
public:
	std::vector<Symbol*>* Symbols;
	std::vector<Statement*>* Code;
};

}
}
}
