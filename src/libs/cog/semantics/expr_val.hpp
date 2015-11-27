#pragma once

#include "cog/ast/ast.hpp"
#include "cog/script/type.hpp"
#include "utility/maybe.hpp"
#include "ast/factory.hpp"
#include "ast/variant_location_visitor.hpp"
#include "log/log.hpp"
#include <tuple>

namespace gorc {
    namespace cog {

        using expr_val = std::tuple<value_type, maybe<value>>;

        template <typename VisitorT>
        value_type visit_and_fold(VisitorT &v, ast::expression &e, ast_factory &fac)
        {
            expr_val ev = ast_visit(v, e);
            maybe_if(std::get<1>(ev), [&](value rv) {
                    auto loc = ast_visit(variant_location_visitor(), e);
                    e = fac.make<ast::immediate_expression>(loc, rv);
                });
            return std::get<0>(ev);
        }

    }
}
