#pragma once

#include <string>
#include <memory>
#include "utility/maybe.hpp"
#include "utility/variant.hpp"

namespace gorc {

    class sexpr_int_type;
    using sexpr_int = std::shared_ptr<sexpr_int_type>;

    class sexpr_leaf_type;
    using sexpr_leaf = std::shared_ptr<sexpr_leaf_type>;

    using sexpr_node = variant<sexpr_int, sexpr_leaf>;
    using sexpr = maybe<sexpr_node>;

    class sexpr_int_type {
    public:
        sexpr left;
        sexpr right;

        sexpr_int_type(sexpr left, sexpr right);
    };

    class sexpr_leaf_type {
    public:
        std::string value;

        sexpr_leaf_type(bool value);
        sexpr_leaf_type(std::string const &value);

        operator bool() const;
        operator std::string() const;
    };

    sexpr make_sexpr(sexpr left, sexpr right);

    template <typename T>
    sexpr make_sexpr(T const &value)
    {
        return sexpr(sexpr_node(std::make_shared<sexpr_leaf_type>(value)));
    }

}
