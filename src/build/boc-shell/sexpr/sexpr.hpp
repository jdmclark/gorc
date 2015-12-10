#pragma once

#include <string>
#include <memory>
#include "utility/maybe.hpp"
#include "utility/variant.hpp"
#include "utility/range.hpp"

namespace gorc {

    enum class sexpr_node_type {
        atom,
        cons
    };

    class sexpr_node {
    public:
        sexpr_node_type type;
        std::string atom;
        std::shared_ptr<sexpr_node> left;
        std::shared_ptr<sexpr_node> right;

        sexpr_node();
        explicit sexpr_node(std::string const &atom);
        explicit sexpr_node(char const *atom);
        explicit sexpr_node(bool atom);

        sexpr_node(std::shared_ptr<sexpr_node> left,
                   std::shared_ptr<sexpr_node> right);
    };

    using sexpr = std::shared_ptr<sexpr_node>;

    sexpr make_sexpr();
    sexpr make_sexpr(sexpr left, sexpr right);

    template <typename T>
    sexpr make_sexpr(T const &value)
    {
        return std::make_shared<sexpr_node>(value);
    }

    bool atom(sexpr);
    bool null(sexpr);

    sexpr car(sexpr);
    sexpr cdr(sexpr);

    template <typename RangeT>
    sexpr make_sexpr_from_range(RangeT rng)
    {
        if(rng.begin() == rng.end()) {
            return make_sexpr();
        }

        auto it = rng.begin();
        sexpr left = make_sexpr(*it);
        return make_sexpr(left, make_sexpr_from_range(make_range(++it, rng.end())));
    }
}
