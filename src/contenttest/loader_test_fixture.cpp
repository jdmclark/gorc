#include "loader_test_fixture.h"
#include <fstream>

LoaderTestFixture::LoaderTestFixture(const boost::filesystem::path& BasePath)
	: nfs(BasePath), FileSystem(nfs), ContentManager(Report, FileSystem) {
	return;
}

LoaderTestFixture::LoaderTestFixture(const Gorc::Content::FileSystem& fs)
	: nfs(""), FileSystem(fs), ContentManager(Report, FileSystem) {
	return;
}

void LoaderTestFixture::PrintErrors() const {
	for(const auto& error : Report) {
		NullUnit::Test_Reporter->CaseExpectationFail(
				NullUnit::Test_Suite_Name,
				NullUnit::Test_Case_Name,
				static_cast<std::string>(error),
				error.Location.filename.generic_string(),
				error.Location.first_line);
	}
}
