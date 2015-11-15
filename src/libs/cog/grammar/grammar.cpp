#include "grammar.hpp"
#include "parser.hpp"
#include "lexer.hpp"

gorc::cog::grammar::grammar(input_stream &file, ast::factory &factory)
    : file(file)
    , factory(factory)
{
    return;
}

gorc::maybe<gorc::cog::ast::translation_unit *> gorc::cog::grammar::parse()
{
    try {
        cog_tokenizer scanner(file);
        return parse_cog(factory, scanner);
    }
    catch(logged_runtime_error const &) {
        return nothing;
    }
}
