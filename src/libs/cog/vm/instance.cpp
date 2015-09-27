#include "instance.hpp"

gorc::cog::instance::instance(script const &cog)
    : cog(cog)
{
    for(auto const &sym : cog.symbols) {
        memory[sym.sequence_number] = sym.default_value;
    }

    return;
}
