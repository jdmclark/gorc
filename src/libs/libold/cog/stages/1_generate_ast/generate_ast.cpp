#include "libold/cog/stages/stages.hpp"
#include "libold/cog/ast/node.hpp"
#include "libold/cog/ast/factory.hpp"
#include "io/input_stream.hpp"

#include "libold/cog/grammar/instance.hpp"

gorc::cog::ast::translation_unit* gorc::cog::stages::generate_ast::generate_ast(input_stream& inputFilestream,
        ast::factory& factory) {
    grammar::instance instance(inputFilestream, factory);
    return instance.parse();
}
