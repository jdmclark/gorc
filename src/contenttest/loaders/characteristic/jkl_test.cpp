#include <nullunit/nullunit.h>

#include "contenttest/loader_test_fixture.h"
#include "contenttest/vfssingleton.h"

#include "content/assets/level.h"

class JklTestFixture : public LoaderTestFixture {
public:
	JklTestFixture() : LoaderTestFixture(VfsSingleton::Get()) {
		return;
	}
};

BeginSuiteFixture(JklTest, JklTestFixture);

Case(jk_01narshadda_test) {
	VfsSingleton::SetEpisode("The Force Within");
	auto lev = TryLoad<Gorc::Content::Assets::Level>("01narshadda.jkl", Compiler);

	AssertResult(2, 682);
}

EndSuite(JklTest);
