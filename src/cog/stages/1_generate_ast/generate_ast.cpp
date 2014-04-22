#include "cog/stages/stages.h"
#include "cog/ast/node.h"
#include "cog/ast/factory.h"
#include "base/diagnostics/report.h"
#include "base/text/source.h"

#include "cog/grammar/instance.h"

gorc::cog::ast::translation_unit* gorc::cog::stages::generate_ast::generate_ast(text::source& inputFilestream,
        diagnostics::report& report, ast::factory& factory) {
    grammar::instance instance(inputFilestream, factory, report);
    return instance.parse();
}
