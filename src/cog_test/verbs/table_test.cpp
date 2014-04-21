#include "test/test.h"
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

void OverloadVerb(int, int) {
    return;
}

void ExtVerb(int, int, int) {
    return;
}

int Prod(int x, int y) {
    return x * y;
}

template <typename T> T ZeroVerb() {
    return T();
}

begin_suite(verb_table_Test);

test_case(Exists) {
    verb_table table;
}

test_case(add_verb) {
    verb_table table;

    table.add_verb("Test", EmptyVerb);

    table.add_verb("Test2", OverloadVerb);
}

test_case(TestClass) {
    verb_table table;
    table.add_verb<int, 0>("Test", TestClass(5));
    verb_id id = table.get_verb("Test");

    std::stack<value> stack;
    int rvalue = table.invoke(id, stack, nullptr);

    assert_eq(rvalue, 5);
}

test_case(OverloadVerb) {
    verb_table table;

    table.add_verb("Test", ZeroVerb<int>);

    try {
        table.add_verb("Test", ZeroVerb<int>);
    }
    catch(verb_redefinition_exception&) {
        return;
    }

    assert_always("Did not throw exception");
}

test_case(VerbRedefinition) {
    verb_table table;

    table.add_verb("Test", EmptyVerb);

    try {
        table.add_verb("Test", EmptyVerb);
    }
    catch(verb_redefinition_exception&) {
        return;
    }

    assert_always("Did not throw exception");
}

test_case(get_verb) {
    verb_table table;

    try {
        table.get_verb("Test");
    }
    catch(undefined_verb_exception&) {
        table.add_verb("Test", EmptyVerb);

        table.get_verb("Test");

        return;
    }

    assert_always("Did not throw exception");
}

test_case(parameter_count) {
    verb_table table;
    table.add_verb("Test", EmptyVerb);

    verb_id id = table.get_verb("Test");

    assert_eq(table.parameter_count(id), 0UL);

    table.add_verb("Test2", ExtVerb);

    id = table.get_verb("Test2");

    assert_eq(table.parameter_count(id), 3UL);
}

test_case(return_type) {
    verb_table table;

    table.add_verb("Void", EmptyVerb);
    verb_id id = table.get_verb("Void");
    assert_eq(table.return_type(id), type::nothing);

    table.add_verb("Int", ZeroVerb<int>);
    id = table.get_verb("Int");
    assert_eq(table.return_type(id), type::integer);

    table.add_verb("floating", ZeroVerb<float>);
    id = table.get_verb("floating");
    assert_eq(table.return_type(id), type::floating);

    table.add_verb("vector", ZeroVerb<gorc::vector<3>>);
    id = table.get_verb("vector");
    assert_eq(table.return_type(id), type::vector);

    table.add_verb("Boolean", ZeroVerb<bool>);
    id = table.get_verb("Boolean");
    assert_eq(table.return_type(id), type::boolean);
}

test_case(invoke) {
    verb_table table;

    table.add_verb("Prod", Prod);
    verb_id id = table.get_verb("Prod");

    std::stack<value> stack;
    stack.push(5.0f);
    stack.push(10);

    assert_le(fabsf(static_cast<float>(table.invoke(id, stack, nullptr)) - 50.0f), 0.0f);
}

test_case(VerbExists) {
    verb_table table;

    assert_true(!table.is_verb_defined("Test"));

    table.add_verb("Test", ZeroVerb<int>);

    assert_true(table.is_verb_defined("Test"));
}

end_suite(verb_table_Test);
