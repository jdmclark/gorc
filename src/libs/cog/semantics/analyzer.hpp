#pragma once

#include "cog/script/script.hpp"
#include "cog/script/verb_table.hpp"
#include "cog/script/constant_table.hpp"
#include "ast/factory.hpp"
#include "cog/ast/ast.hpp"

namespace gorc {
    namespace cog {

        void perform_semantic_analysis(script &out_script,
                                       ast::translation_unit &tu,
                                       verb_table const &verbs,
                                       constant_table const &constants,
                                       gorc::ast_factory &factory);

    }
}
