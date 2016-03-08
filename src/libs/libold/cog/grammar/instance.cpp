#include "instance.hpp"

gorc::cog::grammar::instance::instance(input_stream& inputFilestream,
        ast::factory& ast)
    : inputFilestream(inputFilestream), factory(ast) {

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
