#pragma once

#include "utility/maybe.hpp"
#include "jk/cog/ast/ast.hpp"
#include <string>

namespace gorc {
    namespace cog {

        enum class symbol_extension_type {
            local,
            linkid,
            nolink,
            desc,
            mask
        };

        class get_symbol_extension_type_visitor {
        public:
            maybe<symbol_extension_type> visit(ast::identifier &) const;
        };

    }
}
