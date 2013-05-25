#include "factory.h"

using namespace Gorc::Cog::AST;

StringFragmentField* Factory::MakeStringFragmentField(const char* text, const Diagnostics::ErrorLocation& yyl) {
	MAKE(StringFragmentField);
	val->Value = text;
	return val;
}

IntegerField* Factory::MakeIntegerField(int value, const Diagnostics::ErrorLocation& yyl) {
	MAKE(IntegerField);
	val->Value = value;
	return val;
}

FloatField* Factory::MakeFloatField(float value, const Diagnostics::ErrorLocation& yyl) {
	MAKE(FloatField);
	val->Value = value;
	return val;
}

BareExtension* Factory::MakeBareExtension(const char* name, const Diagnostics::ErrorLocation& yyl) {
	MAKE(BareExtension);
	val->Name = name;
	return val;
}

ValuedExtension* Factory::MakeValuedExtension(const char* name, SymbolField* value, const Diagnostics::ErrorLocation& yyl) {
	MAKE(ValuedExtension);
	val->Name = name;
	val->Value = value;
	return val;
}

Symbol* Factory::MakeSymbol(const char* type, const char* name, std::vector<SymbolExtension*>* extensions, const Diagnostics::ErrorLocation& yyl) {
	MAKE(Symbol);
	val->Type = type;
	val->Name = name;
	val->Extensions = extensions;
	return val;
}

ValuedSymbol* Factory::MakeValuedSymbol(const char* type, const char* name, std::vector<SymbolExtension*>* extensions, SymbolField* value, const Diagnostics::ErrorLocation& yyl) {
	MAKE(ValuedSymbol);
	val->Type = type;
	val->Name = name;
	val->Extensions = extensions;
	val->Value = value;
	return val;
}

ConstantValueExpression* Factory::MakeConstantValueExpression(const VM::Value& value, const Diagnostics::ErrorLocation& yyl) {
	MAKE(ConstantValueExpression);
	val->Value = value;
	return val;
}

StringLiteralExpression* Factory::MakeStringLiteralExpression(const char* value, const Diagnostics::ErrorLocation& yyl) {
	MAKE(StringLiteralExpression);
	val->Value = value;
	return val;
}

IntegerLiteralExpression* Factory::MakeIntegerLiteralExpression(int value, const Diagnostics::ErrorLocation& yyl) {
	MAKE(IntegerLiteralExpression);
	val->Value = value;
	return val;
}

FloatLiteralExpression* Factory::MakeFloatLiteralExpression(float value, const Diagnostics::ErrorLocation& yyl) {
	MAKE(FloatLiteralExpression);
	val->Value = value;
	return val;
}

VectorLiteralExpression* Factory::MakeVectorLiteralExpression(float x, float y, float z, const Diagnostics::ErrorLocation& yyl) {
	MAKE(VectorLiteralExpression);
	val->X = x;
	val->Y = y;
	val->Z = z;
	return val;
}

IdentifierExpression* Factory::MakeIdentifierExpression(const char* identifier, const Diagnostics::ErrorLocation& yyl) {
	MAKE(IdentifierExpression);
	val->Identifier = identifier;
	return val;
}

SubscriptExpression* Factory::MakeSubscriptExpression(const char* base, Expression* index, const Diagnostics::ErrorLocation& yyl) {
	MAKE(SubscriptExpression);
	val->Base = base;
	val->Index = index;
	return val;
}

MethodCallExpression* Factory::MakeMethodCallExpression(const char* base, std::vector<Expression*>* arguments, const Diagnostics::ErrorLocation& yyl) {
	MAKE(MethodCallExpression);
	val->Base = base;
	val->Arguments = arguments;
	return val;
}

UnaryExpression* Factory::MakeUnaryExpression(Expression* base, UnaryOperator op, const Diagnostics::ErrorLocation& yyl) {
	MAKE(UnaryExpression);
	val->Base = base;
	val->Operator = op;
	return val;
}

InfixExpression* Factory::MakeInfixExpression(Expression* left, Expression* right, InfixOperator op, const Diagnostics::ErrorLocation& yyl) {
	MAKE(InfixExpression);
	val->Left = left;
	val->Right = right;
	val->Operator = op;
	return val;
}

AssignmentExpression* Factory::MakeAssignmentExpression(Expression* target, Expression* value, const Diagnostics::ErrorLocation& yyl) {
	MAKE(AssignmentExpression);
	val->Target = target;
	val->Value = value;
	return val;
}

CommaExpression* Factory::MakeCommaExpression(Expression* left, Expression* right, const Diagnostics::ErrorLocation& yyl) {
	MAKE(CommaExpression);
	val->Left = left;
	val->Right = right;
	return val;
}

CompoundStatement* Factory::MakeCompoundStatement(std::vector<Statement*>* code, const Diagnostics::ErrorLocation& yyl) {
	MAKE(CompoundStatement);
	val->Code = code;
	return val;
}

EmptyStatement* Factory::MakeEmptyStatement(const Diagnostics::ErrorLocation& yyl) {
	MAKE(EmptyStatement);
	return val;
}

ExpressionStatement* Factory::MakeExpressionStatement(Expression* expression, const Diagnostics::ErrorLocation& yyl) {
	MAKE(ExpressionStatement);
	val->Expression = expression;
	return val;
}

BreakStatement* Factory::MakeBreakStatement(const Diagnostics::ErrorLocation& yyl) {
	MAKE(BreakStatement);
	return val;
}

ReturnStatement* Factory::MakeReturnStatement(const Diagnostics::ErrorLocation& yyl) {
	MAKE(ReturnStatement);
	return val;
}

CallStatement* Factory::MakeCallStatement(const char* label, const Diagnostics::ErrorLocation& yyl) {
	MAKE(CallStatement);
	val->Label = label;
	return val;
}

IfStatement* Factory::MakeIfStatement(Expression* condition, Statement* code, const Diagnostics::ErrorLocation& yyl) {
	MAKE(IfStatement);
	val->Condition = condition;
	val->Code = code;
	return val;
}

IfElseStatement* Factory::MakeIfElseStatement(Expression* condition, Statement* code, Statement* elsecode, const Diagnostics::ErrorLocation& yyl) {
	MAKE(IfElseStatement);
	val->Condition = condition;
	val->Code = code;
	val->ElseCode = elsecode;
	return val;
}

WhileStatement* Factory::MakeWhileStatement(Expression* condition, Statement* code, const Diagnostics::ErrorLocation& yyl) {
	MAKE(WhileStatement);
	val->Condition = condition;
	val->Code = code;
	return val;
}

DoStatement* Factory::MakeDoStatement(Statement* code, Expression* condition, const Diagnostics::ErrorLocation& yyl) {
	MAKE(DoStatement);
	val->Code = code;
	val->Condition = condition;
	return val;
}

ForOptionalExpression* Factory::MakeForOptionalExpression(const Diagnostics::ErrorLocation& yyl) {
	MAKE(ForOptionalExpression);
	return val;
}

ForExpression* Factory::MakeForExpression(Expression* condition, const Diagnostics::ErrorLocation& yyl) {
	MAKE(ForExpression);
	val->Condition = condition;
	return val;
}

ForStatement* Factory::MakeForStatement(ForOptionalExpression* initializer, ForOptionalExpression* condition, ForOptionalExpression* incrementer, Statement* code, const Diagnostics::ErrorLocation& yyl) {
	MAKE(ForStatement);
	val->Initializer = initializer;
	val->Condition = condition;
	val->Incrementer = incrementer;
	val->Code = code;
	return val;
}

LabeledStatement* Factory::MakeLabeledStatement(const char* label, Statement* code, const Diagnostics::ErrorLocation& yyl) {
	MAKE(LabeledStatement);
	val->Label = label;
	val->Code = code;
	return val;
}

TranslationUnit* Factory::MakeTranslationUnit(std::vector<Symbol*>* symbols, std::vector<Statement*>* code, const Diagnostics::ErrorLocation& yyl) {
	MAKE(TranslationUnit);
	val->Symbols = symbols;
	val->Code = code;
	return val;
}
