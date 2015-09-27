#include "compiler.hpp"
#include "cog/ast/factory.hpp"
#include "cog/grammar/grammar.hpp"
#include "log/log.hpp"
#include "utility/make_unique.hpp"
#include "cog/semantics/analyzer.hpp"
#include "cog/codegen/codegen.hpp"

gorc::cog::compiler::compiler(verb_table &verbs,
                              constant_table &constants)
    : verbs(verbs)
    , constants(constants)
{
    return;
}

std::unique_ptr<gorc::cog::script> gorc::cog::compiler::compile(input_stream &f)
{
    cog::ast::factory ast_factory;
    cog::grammar grammar(f, ast_factory);
    auto maybe_tu = grammar.parse();

    // Abort after any syntax error: the AST is malformed
    LOG_FATAL_ASSERT(maybe_tu.has_value() && diagnostic_file_error_count() == 0,
                     "could not compile script");

    cog::ast::translation_unit *tu = maybe_tu.get_value();
    auto script = make_unique<cog::script>();

    if(!handle_parsed_ast(*tu)) {
        return std::move(script);
    }

    cog::perform_semantic_analysis(*script,
                                   *tu,
                                   verbs,
                                   constants,
                                   ast_factory);

    // Abort after any semantic error
    LOG_FATAL_ASSERT(diagnostic_file_error_count() == 0,
                     "could not compile script");

    if(!handle_analyzed_ast(*tu, *script)) {
        return std::move(script);
    }

    cog::perform_code_generation(*script,
                                 *tu,
                                 verbs,
                                 constants);

    // Abort after any code generation error
    LOG_FATAL_ASSERT(diagnostic_file_error_count() == 0,
                     "could not compile script");

    handle_generated_code(*script);

    return std::move(script);
}
