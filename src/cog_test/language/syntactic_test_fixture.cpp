#include "syntactic_test_fixture.h"
#include "cog/stages/stages.h"
#include "base/io/native_file.h"
#include <fstream>

void SyntacticTestFixture::ParseFile(const boost::filesystem::path& filename) {
    gorc::cog::ast::factory astFactory;

    std::unique_ptr<gorc::io::read_only_file> file;
    try {
        file = FileSystem.open(filename);
    }
    catch(const std::exception& e) {
        report.add_critical_error("SyntacticTestFixture", "could not open file");
        return;
    }

    gorc::text::source source(filename, *file);

    gorc::cog::stages::generate_ast::generate_ast(source, report, astFactory);
}
