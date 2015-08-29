#pragma once

#include "cog/ast/ast.hpp"
#include "cog/script/script.hpp"

namespace gorc {
    namespace cog {

        class symbol_field_visitor {
        private:
            script &output_script;
            value_type expected_type;

        public:
            symbol_field_visitor(script &output_script,
                                 value_type expected_type);

            value visit(ast::string_fragment_field &) const;
            value visit(ast::integer_field &) const;
            value visit(ast::float_field &) const;
        };

    }
}
