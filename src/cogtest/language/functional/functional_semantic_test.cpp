#include <nullunit/nullunit.h>
#include <memory>
#include <fstream>

#include "cogtest/language/semantic_test_fixture.h"

class FunctionalSemanticTestFixture : public SemanticTestFixture
{
public:
	FunctionalSemanticTestFixture()
		: SemanticTestFixture("test/cog/functional/semantic") {
		return;
	}
};

BeginSuiteFixture(FunctionalSemanticTest, FunctionalSemanticTestFixture);

Case(s2_sym_ext_invalid) {
	ParseFile("sym_ext_invalid.cog");
	AssertResult(0, 4);
}

Case(s2_sym_ext_invalid_params) {
	ParseFile("sym_ext_invalid_params.cog");
	AssertResult(11, 0);
}

Case(s2_sym_ext_passing) {
	ParseFile("sym_ext_passing.cog");
	AssertResult(0, 0);
}

Case(s2_sym_ext_redefinition) {
	ParseFile("sym_ext_redefinition.cog");
	AssertResult(0, 1);
}

Case(s2_sym_ext_valid_params) {
	ParseFile("sym_ext_valid_params.cog");
	AssertResult(0, 0);
}

Case(s2_sym_redefinition) {
	ParseFile("sym_redefinition.cog");
	AssertResult(0, 1);
}

Case(s2_sym_undefined) {
	ParseFile("sym_undefined.cog");
	AssertResult(0, 1);
}

Case(s2_verb_invalid_overload) {
	ParseFile("verb_invalid_overload.cog");
	AssertResult(1, 0);
}

Case(s2_verb_passing) {
	VerbTable.AddVerb<void, 0>("testverb", [](){});

	ParseFile("verb_passing.cog");
	AssertResult(0, 0);
}

Case(s2_verb_undefined) {
	ParseFile("verb_undefined.cog");
	AssertResult(1, 0);
}

EndSuite(FunctionalSemanticTest);
