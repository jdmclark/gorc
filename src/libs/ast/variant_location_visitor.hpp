#pragma once

#include "node.hpp"

namespace gorc {

    class variant_location_visitor {
    public:
        diagnostic_context_location visit(ast_node &) const;
    };

}
