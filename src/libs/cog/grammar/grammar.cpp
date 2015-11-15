#include "grammar.hpp"
#include "parser.hpp"

gorc::cog::grammar::grammar(input_stream &file, ast::factory &factory)
    : file(file)
    , factory(factory)
    , scanner(file)
{
    return;
}

gorc::maybe<gorc::cog::ast::translation_unit *> gorc::cog::grammar::parse()
{
    return parse_cog(factory, scanner);
}
