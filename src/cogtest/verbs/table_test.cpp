#include <nullunit/nullunit.h>
#include "cog/verbs/table.h"
#include "cog/verbs/exception.h"
#include "cog/vm/value.h"

using namespace Gorc::Cog::Verbs;
using namespace Gorc::Cog::VM;

void EmptyVerb() {
	return;
}

class TestClass {
public:
	int var;

	TestClass(int var) : var(var) {
		return;
	}

	int operator()() {
		return var;
	}
};

void OverloadVerb(int a, int b) {
	return;
}

void ExtVerb(int a, int b, int c) {
	return;
}

int Prod(int x, int y) {
	return x * y;
}

template <typename T> T ZeroVerb() {
	return T();
}

BeginSuite(VerbTable_Test);

Case(Exists) {
	VerbTable table;
}

Case(AddVerb) {
	VerbTable table;

	table.AddVerb("Test", EmptyVerb);

	table.AddVerb("Test2", OverloadVerb);
}

Case(TestClass) {
	VerbTable table;
	table.AddVerb<int, 0>("Test", TestClass(5));
	VerbId id = table.GetVerb("Test");

	std::stack<Value> stack;
	int rvalue = table.Invoke(id, stack);

	Test_Assert_Eq(rvalue, 5);
}

Case(OverloadVerb) {
	VerbTable table;

	table.AddVerb("Test", ZeroVerb<int>);

	try {
		table.AddVerb("Test", ZeroVerb<int>);
	}
	catch(VerbRedefinitionException&) {
		return;
	}

	Test_Assert_Always("Did not throw exception");
}

Case(VerbRedefinition) {
	VerbTable table;

	table.AddVerb("Test", EmptyVerb);

	try {
		table.AddVerb("Test", EmptyVerb);
	}
	catch(VerbRedefinitionException&) {
		return;
	}

	Test_Assert_Always("Did not throw exception");
}

Case(GetVerb) {
	VerbTable table;

	try {
		table.GetVerb("Test");
	}
	catch(UndefinedVerbException&) {
		table.AddVerb("Test", EmptyVerb);

		VerbId id = table.GetVerb("Test");

		return;
	}

	Test_Assert_Always("Did not throw exception");
}

Case(ParameterCount) {
	VerbTable table;
	table.AddVerb("Test", EmptyVerb);

	VerbId id = table.GetVerb("Test");

	Test_Assert_Eq(table.ParameterCount(id), 0);

	table.AddVerb("Test2", ExtVerb);

	id = table.GetVerb("Test2");

	Test_Assert_Eq(table.ParameterCount(id), 3);
}

Case(ReturnType) {
	VerbTable table;

	table.AddVerb("Void", EmptyVerb);
	VerbId id = table.GetVerb("Void");
	Test_Assert_Eq(table.ReturnType(id), Type::Void);

	table.AddVerb("Int", ZeroVerb<int>);
	id = table.GetVerb("Int");
	Test_Assert_Eq(table.ReturnType(id), Type::Integer);

	table.AddVerb("Float", ZeroVerb<float>);
	id = table.GetVerb("Float");
	Test_Assert_Eq(table.ReturnType(id), Type::Float);

	table.AddVerb("Vector", ZeroVerb<Gorc::Math::Vector<float>>);
	id = table.GetVerb("Vector");
	Test_Assert_Eq(table.ReturnType(id), Type::Vector);

	table.AddVerb("Boolean", ZeroVerb<bool>);
	id = table.GetVerb("Boolean");
	Test_Assert_Eq(table.ReturnType(id), Type::Boolean);
}

Case(Invoke) {
	VerbTable table;

	table.AddVerb("Prod", Prod);
	VerbId id = table.GetVerb("Prod");

	std::stack<Value> stack;
	stack.push(5.0f);
	stack.push(10);

	Test_Assert_Eq(static_cast<float>(table.Invoke(id, stack)), 50);
}

Case(VerbExists) {
	VerbTable table;

	Test_Assert(!table.IsVerbDefined("Test"));

	table.AddVerb("Test", ZeroVerb<int>);

	Test_Assert(table.IsVerbDefined("Test"));
}

EndSuite(VerbTable_Test);
