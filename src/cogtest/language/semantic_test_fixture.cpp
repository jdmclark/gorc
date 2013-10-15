#include "semantic_test_fixture.h"
#include "cog/stages/stages.h"
#include "framework/io/native_file.h"
#include <fstream>

void SemanticTestFixture::ParseFile(const boost::filesystem::path& filename) {
	gorc::cog::ast::factory astFactory;

	std::unique_ptr<gorc::io::read_only_file> file;
	try {
		file = FileSystem.open(filename);
	}
	catch(const std::exception& e) {
		report.add_critical_error("SemanticTestFixture", "could not open file");
		return;
	}

	gorc::text::source source(*file);

	gorc::cog::ast::translation_unit* ast = gorc::cog::stages::generate_ast::generate_ast(source, report, astFactory);

	if(report.get_error_count() != 0) {
		return;
	}

	gorc::cog::stages::semantic_analysis::semantic_analysis(ast, symbol_table, ConstantTable, verb_table, report);
}
