#pragma once

#include "libold/cog/ast/node.hpp"
#include <string>

namespace gorc {
namespace cog {
namespace stages {
namespace semantic_analysis {

class symbol_field_visitor {
public:
    float float_value;
    bool is_float;

    int int_value;
    bool is_int;

    bool is_numeric;

    std::string str_value;
    bool is_str;

    symbol_field_visitor();

    void visit(ast::string_fragment_field &);
    void visit(ast::integer_field &);
    void visit(ast::float_field &);
};

}
}
}
}
