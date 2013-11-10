#include "language_test_fixture.h"
#include "cog/stages/stages.h"
#include <fstream>

LanguageTestFixture::LanguageTestFixture(const boost::filesystem::path& BasePath)
	: nfs(BasePath), FileSystem(nfs) {
	return;
}

LanguageTestFixture::LanguageTestFixture(const gorc::content::filesystem& fs)
	: nfs(""), FileSystem(fs) {
	return;
}

void LanguageTestFixture::PrintErrors() const {
	for(const auto& error : report) {
		NullUnit::Test_Reporter->CaseExpectationFail(
				NullUnit::Test_Suite_Name,
				NullUnit::Test_Case_Name,
				static_cast<std::string>(error),
				error.location.filename.generic_string(),
				error.location.first_line);
	}
}
