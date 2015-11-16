#pragma once

#include "cog/ast/ast.hpp"

namespace gorc {
    namespace cog {

        class variant_location_visitor {
        public:
            diagnostic_context_location visit(ast::node &) const;
        };

    }
}
