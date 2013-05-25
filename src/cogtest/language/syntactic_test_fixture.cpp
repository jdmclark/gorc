#include "syntactic_test_fixture.h"
#include "cog/stages/stages.h"
#include "framework/io/nativefile.h"
#include <fstream>

SyntacticTestFixture::SyntacticTestFixture(const boost::filesystem::path& BasePath)
	: LanguageTestFixture(BasePath) {
	return;
}

void SyntacticTestFixture::ParseFile(const boost::filesystem::path& filename) {
	Gorc::Cog::AST::Factory astFactory;

	Gorc::IO::NativeFile file(BasePath / filename);
	Gorc::Text::Source source(file);

	Gorc::Cog::Stages::GenerateAST::GenerateAST(source, Report, astFactory);
}
