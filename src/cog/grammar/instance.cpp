#include "instance.h"

gorc::cog::grammar::instance::instance(text::source& inputFilestream, ast::factory& ast, diagnostics::report& report)
    : inputFilestream(inputFilestream), factory(ast), report(report) {
    std::string* stored_filename = ast.store_value(inputFilestream.filename);
    current_filename = stored_filename->c_str();

    init_scanner();
    return;
}

gorc::cog::grammar::instance::~instance() {
    destroy_scanner();
}

int gra_parse(gorc::cog::grammar::instance*);

gorc::cog::ast::translation_unit* gorc::cog::grammar::instance::parse() {
    gra_parse(this);
    return return_value;
}
