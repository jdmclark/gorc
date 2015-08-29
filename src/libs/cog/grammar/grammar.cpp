#include "grammar.hpp"

gorc::cog::grammar::grammar(input_stream &file, ast::factory &factory)
    : file(file)
    , factory(factory)
{
    init_scanner();
    return;
}

gorc::cog::grammar::~grammar()
{
    destroy_scanner();
}

char gorc::cog::grammar::get_next()
{
    if(!file.at_end()) {
        try {
            return read<char>(file);
        }
        catch(...) {
            // Ignore
        }
    }

    return '\0';
}

void* gorc::cog::grammar::get_scanner() const
{
    return scanner;
}
