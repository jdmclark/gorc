#include "test/test.h"
#include <memory>
#include <fstream>
#include <queue>

#include "cog_test/language/codegen_test_fixture.h"

class FunctionalCodegenTestFixture : public CodegenTestFixture {
public:
    std::queue<int> Output;

    inline int Pop() {
        int value = Output.front();
        Output.pop();
        return value;
    }

    inline bool IsEmpty() const {
        return Output.empty();
    }

    class printint_functor {
    private:
        std::queue<int>& output;

    public:
        printint_functor(std::queue<int>& output)
            : output(output) {
            return;
        }

        void operator()(int n) {
            output.push(n);
        }
    };

    class printint3_functor {
    private:
        std::queue<int>& output;

    public:
        printint3_functor(std::queue<int>& output)
            : output(output) {
            return;
        }

        void operator()(int a, int b, int c) {
            output.push(a);
            output.push(b);
            output.push(c);
        }
    };

    FunctionalCodegenTestFixture()
        : CodegenTestFixture("test/cog/functional/codegen") {
        verb_table.add_verb<void, 1>("printint", printint_functor(Output));
        verb_table.add_verb<void, 3>("printint3", printint3_functor(Output));
        verb_table.add_verb<int, 0>("getint", [](){ return 5847; });
        return;
    }
};

begin_suite_fixture(FunctionalCodegenTest, FunctionalCodegenTestFixture);

test_case(s2_arithmetic) {
    ParseFile("arithmetic.cog");
    AssertResult(0, 0);

    assert_eq(Pop(), 0);
    assert_eq(Pop(), -1);
    assert_eq(Pop(), 1);

    assert_eq(Pop(), 0);
    assert_eq(Pop(), 1);
    assert_eq(Pop(), 1);

    assert_eq(Pop(), -1);
    assert_eq(Pop(), -1);

    assert_eq(Pop(), 0);
    assert_eq(Pop(), 1);
    assert_eq(Pop(), 1);
    assert_eq(Pop(), 2);

    assert_eq(Pop(), 0);
    assert_eq(Pop(), -1);
    assert_eq(Pop(), 1);
    assert_eq(Pop(), 0);

    assert_eq(Pop(), 0);
    assert_eq(Pop(), 0);
    assert_eq(Pop(), 1);
    assert_eq(Pop(), 4);
    assert_eq(Pop(), -2);
    assert_eq(Pop(), -2);
    assert_eq(Pop(), 2);

    assert_eq(Pop(), 0);
    assert_eq(Pop(), 1);
    assert_eq(Pop(), 0);
    assert_eq(Pop(), 2);
    assert_eq(Pop(), 2);
    assert_eq(Pop(), -2);
    assert_eq(Pop(), -2);
    assert_eq(Pop(), 2);

    assert_eq(Pop(), 0);
    assert_eq(Pop(), 1);
    assert_eq(Pop(), 1);

    assert_true(IsEmpty());
}

test_case(s2_assignment) {
    ParseFile("assignment.cog");
    AssertResult(0, 0);

    assert_eq(Pop(), 0);

    assert_eq(Pop(), 1);

    assert_eq(Pop(), 1);
    assert_eq(Pop(), 1);

    assert_eq(Pop(), 1);
    assert_eq(Pop(), 1);

    assert_eq(Pop(), 2);

    assert_eq(Pop(), 2);

    assert_eq(Pop(), 0);
    assert_eq(Pop(), 1);
    assert_eq(Pop(), 2);

    assert_eq(Pop(), 2);
    assert_eq(Pop(), 2);
    assert_eq(Pop(), 2);

    assert_true(IsEmpty());
}

test_case(s2_bitwise) {
    ParseFile("bitwise.cog");
    AssertResult(0, 0);

    assert_eq(Pop(), 1);
    assert_eq(Pop(), 3);
    assert_eq(Pop(), 2);
    assert_eq(Pop(), -1);
}

test_case(s2_call) {
    ParseFile("call.cog");
    AssertResult(0, 0);

    assert_eq(Pop(), 1);
    assert_eq(Pop(), 1);
    assert_eq(Pop(), 2);
    assert_eq(Pop(), 6);
    assert_eq(Pop(), 24);
    assert_eq(Pop(), 120);
    assert_true(IsEmpty());
}

test_case(s2_defaults) {
    ParseFile("defaults.cog");
    AssertResult(0, 0);

    assert_eq(Pop(), 5);
    assert_eq(Pop(), 10);
    assert_eq(Pop(), 15);
    assert_eq(Pop(), 20);
    assert_true(IsEmpty());
}

test_case(s2_do) {
    ParseFile("do.cog");
    AssertResult(0, 0);

    assert_eq(Pop(), 32);
    assert_eq(Pop(), 5);
    assert_eq(Pop(), 6);
    assert_eq(Pop(), 7);
    assert_eq(Pop(), 8);
    assert_eq(Pop(), 9);
    assert_eq(Pop(), 10);
    assert_eq(Pop(), 500);
    assert_eq(Pop(), 1000);
    assert_true(IsEmpty());
}

test_case(s2_empty) {
    ParseFile("empty.cog");
    AssertResult(0, 0);

    assert_true(IsEmpty());
}

test_case(s2_for) {
    ParseFile("for.cog");
    AssertResult(0, 0);

    assert_eq(Pop(), 2);
    assert_eq(Pop(), 2);
    assert_eq(Pop(), 2);

    assert_eq(Pop(), 3);
    assert_eq(Pop(), 3);
    assert_eq(Pop(), 3);

    assert_eq(Pop(), 4);
    assert_eq(Pop(), 4);
    assert_eq(Pop(), 4);

    assert_true(IsEmpty());
}

test_case(s2_if) {
    ParseFile("if.cog");
    AssertResult(0, 14);

    assert_eq(Pop(), 1);
    assert_eq(Pop(), 3);
    assert_eq(Pop(), 6);
    assert_eq(Pop(), 7);
    assert_eq(Pop(), 9);
    assert_eq(Pop(), 12);
    assert_eq(Pop(), 17);
    assert_true(IsEmpty());
}

test_case(s2_label) {
    ParseFile("label.cog");
    AssertResult(0, 0);

    assert_eq(Pop(), 16);
    assert_true(IsEmpty());
}

test_case(s2_logical_and) {
    ParseFile("logical_and.cog");
    AssertResult(0, 8);

    assert_eq(Pop(), 1);
    assert_true(IsEmpty());
}

test_case(s2_logical_not) {
    ParseFile("logical_not.cog");
    AssertResult(0, 6);

    assert_eq(Pop(), 0);
    assert_eq(Pop(), 2);
    assert_eq(Pop(), 4);
    assert_true(IsEmpty());
}

test_case(s2_logical_or) {
    ParseFile("logical_or.cog");
    AssertResult(0, 8);

    assert_eq(Pop(), 1);
    assert_eq(Pop(), 2);
    assert_eq(Pop(), 3);
    assert_true(IsEmpty());
}

test_case(s2_relation) {
    ParseFile("relation.cog");
    AssertResult(0, 0);

    assert_eq(Pop(), 1);
    assert_eq(Pop(), 4);

    assert_eq(Pop(), 6);
    assert_eq(Pop(), 7);

    assert_eq(Pop(), 11);

    assert_eq(Pop(), 13);
    assert_eq(Pop(), 15);
    assert_eq(Pop(), 16);

    assert_eq(Pop(), 18);

    assert_eq(Pop(), 21);
    assert_eq(Pop(), 22);
    assert_eq(Pop(), 24);

    assert_true(IsEmpty());
}

test_case(s2_return) {
    ParseFile("return.cog");
    AssertResult(0, 0);

    assert_eq(Pop(), 1);
    assert_true(IsEmpty());
}

test_case(s2_verb) {
    ParseFile("verb.cog");
    AssertResult(0, 0);

    assert_eq(Pop(), 5);
    assert_true(IsEmpty());
}

test_case(s2_verb_param_order) {
    ParseFile("verb_param_order.cog");
    AssertResult(0, 0);

    assert_eq(Pop(), 1);
    assert_eq(Pop(), 2);
    assert_eq(Pop(), 3);
    assert_true(IsEmpty());
}

test_case(s2_verb_return) {
    ParseFile("verb_return.cog");
    AssertResult(0, 0);

    assert_eq(Pop(), 5847);
    assert_true(IsEmpty());
}

test_case(s2_while) {
    ParseFile("while.cog");
    AssertResult(0, 0);

    assert_eq(Pop(), 5);
    assert_eq(Pop(), 6);
    assert_eq(Pop(), 7);
    assert_eq(Pop(), 8);
    assert_eq(Pop(), 9);
    assert_eq(Pop(), 10);
    assert_eq(Pop(), 500);
    assert_true(IsEmpty());
}

end_suite(FunctionalCodegenTest);
