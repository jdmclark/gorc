#include "syntactic_test_fixture.h"
#include "cog/stages/stages.h"
#include "framework/io/nativefile.h"
#include <fstream>

void SyntacticTestFixture::ParseFile(const boost::filesystem::path& filename) {
	Gorc::Cog::AST::Factory astFactory;

	std::unique_ptr<Gorc::IO::ReadOnlyFile> file;
	try {
		file = FileSystem.Open(filename);
	}
	catch(const std::exception& e) {
		Report.AddCriticalError("SyntacticTestFixture", "could not open file");
		return;
	}

	Gorc::Text::Source source(*file);

	Gorc::Cog::Stages::GenerateAST::GenerateAST(source, Report, astFactory);
}
