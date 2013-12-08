#include <nullunit/nullunit.h>
#include "cog/verbs/table.h"
#include "cog/verbs/exception.h"
#include "cog/vm/value.h"

using namespace gorc::cog::verbs;
using namespace gorc::cog::vm;

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

BeginSuite(verb_table_Test);

Case(Exists) {
	verb_table table;
}

Case(add_verb) {
	verb_table table;

	table.add_verb("Test", EmptyVerb);

	table.add_verb("Test2", OverloadVerb);
}

Case(TestClass) {
	verb_table table;
	table.add_verb<int, 0>("Test", TestClass(5));
	verb_id id = table.get_verb("Test");

	std::stack<value> stack;
	int rvalue = table.invoke(id, stack, nullptr);

	Test_Assert_Eq(rvalue, 5);
}

Case(OverloadVerb) {
	verb_table table;

	table.add_verb("Test", ZeroVerb<int>);

	try {
		table.add_verb("Test", ZeroVerb<int>);
	}
	catch(verb_redefinition_exception&) {
		return;
	}

	Test_Assert_Always("Did not throw exception");
}

Case(VerbRedefinition) {
	verb_table table;

	table.add_verb("Test", EmptyVerb);

	try {
		table.add_verb("Test", EmptyVerb);
	}
	catch(verb_redefinition_exception&) {
		return;
	}

	Test_Assert_Always("Did not throw exception");
}

Case(get_verb) {
	verb_table table;

	try {
		table.get_verb("Test");
	}
	catch(undefined_verb_exception&) {
		table.add_verb("Test", EmptyVerb);

		verb_id id = table.get_verb("Test");

		return;
	}

	Test_Assert_Always("Did not throw exception");
}

Case(parameter_count) {
	verb_table table;
	table.add_verb("Test", EmptyVerb);

	verb_id id = table.get_verb("Test");

	Test_Assert_Eq(table.parameter_count(id), 0);

	table.add_verb("Test2", ExtVerb);

	id = table.get_verb("Test2");

	Test_Assert_Eq(table.parameter_count(id), 3);
}

Case(return_type) {
	verb_table table;

	table.add_verb("Void", EmptyVerb);
	verb_id id = table.get_verb("Void");
	Test_Assert_Eq(table.return_type(id), type::nothing);

	table.add_verb("Int", ZeroVerb<int>);
	id = table.get_verb("Int");
	Test_Assert_Eq(table.return_type(id), type::integer);

	table.add_verb("floating", ZeroVerb<float>);
	id = table.get_verb("floating");
	Test_Assert_Eq(table.return_type(id), type::floating);

	table.add_verb("vector", ZeroVerb<gorc::vector<3>>);
	id = table.get_verb("vector");
	Test_Assert_Eq(table.return_type(id), type::vector);

	table.add_verb("Boolean", ZeroVerb<bool>);
	id = table.get_verb("Boolean");
	Test_Assert_Eq(table.return_type(id), type::boolean);
}

Case(invoke) {
	verb_table table;

	table.add_verb("Prod", Prod);
	verb_id id = table.get_verb("Prod");

	std::stack<value> stack;
	stack.push(5.0f);
	stack.push(10);

	Test_Assert_Eq(static_cast<float>(table.invoke(id, stack, nullptr)), 50);
}

Case(VerbExists) {
	verb_table table;

	Test_Assert(!table.is_verb_defined("Test"));

	table.add_verb("Test", ZeroVerb<int>);

	Test_Assert(table.is_verb_defined("Test"));
}

EndSuite(verb_table_Test);
