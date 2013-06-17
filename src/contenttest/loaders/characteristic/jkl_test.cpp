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

	/*auto f = VfsSingleton::Get().Open("cog/xcrane.cog");
	std::vector<char> buffer(f->GetSize());
	f->Read(&buffer[0], f->GetSize());
	buffer.push_back('\0');
	std::cout << &buffer[0] << std::endl;*/

	auto lev = TryLoad<Gorc::Content::Assets::Level>("01narshadda.jkl", Compiler);
	AssertResult(2, 144);
}

EndSuite(JklTest);
