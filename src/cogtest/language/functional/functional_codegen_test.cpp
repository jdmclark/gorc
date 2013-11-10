#include <nullunit/nullunit.h>
#include <memory>
#include <fstream>
#include <queue>

#include "cogtest/language/codegen_test_fixture.h"

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

BeginSuiteFixture(FunctionalCodegenTest, FunctionalCodegenTestFixture);

Case(s2_arithmetic) {
	ParseFile("arithmetic.cog");
	AssertResult(0, 0);

	Test_Assert_Eq(Pop(), 0);
	Test_Assert_Eq(Pop(), -1);
	Test_Assert_Eq(Pop(), 1);

	Test_Assert_Eq(Pop(), 0);
	Test_Assert_Eq(Pop(), 1);
	Test_Assert_Eq(Pop(), 1);

	Test_Assert_Eq(Pop(), -1);
	Test_Assert_Eq(Pop(), -1);

	Test_Assert_Eq(Pop(), 0);
	Test_Assert_Eq(Pop(), 1);
	Test_Assert_Eq(Pop(), 1);
	Test_Assert_Eq(Pop(), 2);

	Test_Assert_Eq(Pop(), 0);
	Test_Assert_Eq(Pop(), -1);
	Test_Assert_Eq(Pop(), 1);
	Test_Assert_Eq(Pop(), 0);

	Test_Assert_Eq(Pop(), 0);
	Test_Assert_Eq(Pop(), 0);
	Test_Assert_Eq(Pop(), 1);
	Test_Assert_Eq(Pop(), 4);
	Test_Assert_Eq(Pop(), -2);
	Test_Assert_Eq(Pop(), -2);
	Test_Assert_Eq(Pop(), 2);

	Test_Assert_Eq(Pop(), 0);
	Test_Assert_Eq(Pop(), 1);
	Test_Assert_Eq(Pop(), 0);
	Test_Assert_Eq(Pop(), 2);
	Test_Assert_Eq(Pop(), 2);
	Test_Assert_Eq(Pop(), -2);
	Test_Assert_Eq(Pop(), -2);
	Test_Assert_Eq(Pop(), 2);

	Test_Assert_Eq(Pop(), 0);
	Test_Assert_Eq(Pop(), 1);
	Test_Assert_Eq(Pop(), 1);

	Test_Assert(IsEmpty());
}

Case(s2_assignment) {
	ParseFile("assignment.cog");
	AssertResult(0, 0);

	Test_Assert_Eq(Pop(), 0);

	Test_Assert_Eq(Pop(), 1);

	Test_Assert_Eq(Pop(), 1);
	Test_Assert_Eq(Pop(), 1);

	Test_Assert_Eq(Pop(), 1);
	Test_Assert_Eq(Pop(), 1);

	Test_Assert_Eq(Pop(), 2);

	Test_Assert_Eq(Pop(), 2);

	Test_Assert_Eq(Pop(), 0);
	Test_Assert_Eq(Pop(), 1);
	Test_Assert_Eq(Pop(), 2);

	Test_Assert_Eq(Pop(), 2);
	Test_Assert_Eq(Pop(), 2);
	Test_Assert_Eq(Pop(), 2);

	Test_Assert(IsEmpty());
}

Case(s2_bitwise) {
	ParseFile("bitwise.cog");
	AssertResult(0, 0);

	Test_Assert_Eq(Pop(), 1);
	Test_Assert_Eq(Pop(), 3);
	Test_Assert_Eq(Pop(), 2);
	Test_Assert_Eq(Pop(), -1);
}

Case(s2_call) {
	ParseFile("call.cog");
	AssertResult(0, 0);

	Test_Assert_Eq(Pop(), 1);
	Test_Assert_Eq(Pop(), 1);
	Test_Assert_Eq(Pop(), 2);
	Test_Assert_Eq(Pop(), 6);
	Test_Assert_Eq(Pop(), 24);
	Test_Assert_Eq(Pop(), 120);
	Test_Assert(IsEmpty());
}

Case(s2_defaults) {
	ParseFile("defaults.cog");
	AssertResult(0, 0);

	Test_Assert_Eq(Pop(), 5);
	Test_Assert_Eq(Pop(), 10);
	Test_Assert_Eq(Pop(), 15);
	Test_Assert_Eq(Pop(), 20);
	Test_Assert(IsEmpty());
}

Case(s2_do) {
	ParseFile("do.cog");
	AssertResult(0, 0);

	Test_Assert_Eq(Pop(), 32);
	Test_Assert_Eq(Pop(), 5);
	Test_Assert_Eq(Pop(), 6);
	Test_Assert_Eq(Pop(), 7);
	Test_Assert_Eq(Pop(), 8);
	Test_Assert_Eq(Pop(), 9);
	Test_Assert_Eq(Pop(), 10);
	Test_Assert_Eq(Pop(), 500);
	Test_Assert_Eq(Pop(), 1000);
	Test_Assert(IsEmpty());
}

Case(s2_empty) {
	ParseFile("empty.cog");
	AssertResult(0, 0);

	Test_Assert(IsEmpty());
}

Case(s2_for) {
	ParseFile("for.cog");
	AssertResult(0, 0);

	Test_Assert_Eq(Pop(), 2);
	Test_Assert_Eq(Pop(), 2);
	Test_Assert_Eq(Pop(), 2);

	Test_Assert_Eq(Pop(), 3);
	Test_Assert_Eq(Pop(), 3);
	Test_Assert_Eq(Pop(), 3);

	Test_Assert_Eq(Pop(), 4);
	Test_Assert_Eq(Pop(), 4);
	Test_Assert_Eq(Pop(), 4);

	Test_Assert(IsEmpty());
}

Case(s2_if) {
	ParseFile("if.cog");
	AssertResult(0, 14);

	Test_Assert_Eq(Pop(), 1);
	Test_Assert_Eq(Pop(), 3);
	Test_Assert_Eq(Pop(), 6);
	Test_Assert_Eq(Pop(), 7);
	Test_Assert_Eq(Pop(), 9);
	Test_Assert_Eq(Pop(), 12);
	Test_Assert_Eq(Pop(), 17);
	Test_Assert(IsEmpty());
}

Case(s2_label) {
	ParseFile("label.cog");
	AssertResult(0, 0);

	Test_Assert_Eq(Pop(), 16);
	Test_Assert(IsEmpty());
}

Case(s2_logical_and) {
	ParseFile("logical_and.cog");
	AssertResult(0, 8);

	Test_Assert_Eq(Pop(), 1);
	Test_Assert(IsEmpty());
}

Case(s2_logical_not) {
	ParseFile("logical_not.cog");
	AssertResult(0, 6);

	Test_Assert_Eq(Pop(), 0);
	Test_Assert_Eq(Pop(), 2);
	Test_Assert_Eq(Pop(), 4);
	Test_Assert(IsEmpty());
}

Case(s2_logical_or) {
	ParseFile("logical_or.cog");
	AssertResult(0, 8);

	Test_Assert_Eq(Pop(), 1);
	Test_Assert_Eq(Pop(), 2);
	Test_Assert_Eq(Pop(), 3);
	Test_Assert(IsEmpty());
}

Case(s2_relation) {
	ParseFile("relation.cog");
	AssertResult(0, 0);

	Test_Assert_Eq(Pop(), 1);
	Test_Assert_Eq(Pop(), 4);

	Test_Assert_Eq(Pop(), 6);
	Test_Assert_Eq(Pop(), 7);

	Test_Assert_Eq(Pop(), 11);

	Test_Assert_Eq(Pop(), 13);
	Test_Assert_Eq(Pop(), 15);
	Test_Assert_Eq(Pop(), 16);

	Test_Assert_Eq(Pop(), 18);

	Test_Assert_Eq(Pop(), 21);
	Test_Assert_Eq(Pop(), 22);
	Test_Assert_Eq(Pop(), 24);

	Test_Assert(IsEmpty());
}

Case(s2_return) {
	ParseFile("return.cog");
	AssertResult(0, 0);

	Test_Assert_Eq(Pop(), 1);
	Test_Assert(IsEmpty());
}

Case(s2_verb) {
	ParseFile("verb.cog");
	AssertResult(0, 0);

	Test_Assert_Eq(Pop(), 5);
	Test_Assert(IsEmpty());
}

Case(s2_verb_param_order) {
	ParseFile("verb_param_order.cog");
	AssertResult(0, 0);

	Test_Assert_Eq(Pop(), 1);
	Test_Assert_Eq(Pop(), 2);
	Test_Assert_Eq(Pop(), 3);
	Test_Assert(IsEmpty());
}

Case(s2_verb_return) {
	ParseFile("verb_return.cog");
	AssertResult(0, 0);

	Test_Assert_Eq(Pop(), 5847);
	Test_Assert(IsEmpty());
}

Case(s2_while) {
	ParseFile("while.cog");
	AssertResult(0, 0);

	Test_Assert_Eq(Pop(), 5);
	Test_Assert_Eq(Pop(), 6);
	Test_Assert_Eq(Pop(), 7);
	Test_Assert_Eq(Pop(), 8);
	Test_Assert_Eq(Pop(), 9);
	Test_Assert_Eq(Pop(), 10);
	Test_Assert_Eq(Pop(), 500);
	Test_Assert(IsEmpty());
}

EndSuite(FunctionalCodegenTest);
