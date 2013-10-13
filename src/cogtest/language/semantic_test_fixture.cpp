#include "semantic_test_fixture.h"
#include "cog/stages/stages.h"
#include "framework/io/native_file.h"
#include <fstream>

void SemanticTestFixture::ParseFile(const boost::filesystem::path& filename) {
	Gorc::Cog::AST::Factory astFactory;

	std::unique_ptr<Gorc::IO::ReadOnlyFile> file;
	try {
		file = FileSystem.Open(filename);
	}
	catch(const std::exception& e) {
		Report.AddCriticalError("SemanticTestFixture", "could not open file");
		return;
	}

	Gorc::Text::Source source(*file);

	Gorc::Cog::AST::TranslationUnit* ast = Gorc::Cog::Stages::GenerateAST::GenerateAST(source, Report, astFactory);

	if(Report.GetErrorCount() != 0) {
		return;
	}

	Gorc::Cog::Stages::SemanticAnalysis::SemanticAnalysis(ast, SymbolTable, ConstantTable, VerbTable, Report);
}
