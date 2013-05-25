#include "semantic_test_fixture.h"
#include "cog/stages/stages.h"
#include "framework/io/nativefile.h"
#include <fstream>

SemanticTestFixture::SemanticTestFixture(const boost::filesystem::path& BasePath)
	: LanguageTestFixture(BasePath) {
	return;
}

void SemanticTestFixture::ParseFile(const boost::filesystem::path& filename) {
	Gorc::Cog::AST::Factory astFactory;

	Gorc::IO::NativeFile file(BasePath / filename);
	Gorc::Text::Source source(file);

	Gorc::Cog::AST::TranslationUnit* ast = Gorc::Cog::Stages::GenerateAST::GenerateAST(source, Report, astFactory);

	if(Report.GetErrorCount() != 0) {
		return;
	}

	Gorc::Cog::Stages::SemanticAnalysis::SemanticAnalysis(ast, SymbolTable, ConstantTable, VerbTable, Report);
}
