#include "sexpr.hpp"
#include "log/log.hpp"

gorc::sexpr_node::sexpr_node()
    : type(sexpr_node_type::atom)
{
    return;
}

gorc::sexpr_node::sexpr_node(std::string const &atom)
    : type(sexpr_node_type::atom)
    , atom(atom)
{
    return;
}

gorc::sexpr_node::sexpr_node(char const *atom)
    : type(sexpr_node_type::atom)
    , atom(atom)
{
    return;
}

gorc::sexpr_node::sexpr_node(bool value)
    : type(sexpr_node_type::atom)
    , atom(value ? "true" : "false")
{
    return;
}

gorc::sexpr_node::sexpr_node(sexpr left, sexpr right)
    : type(sexpr_node_type::cons)
    , left(left)
    , right(right)
{
    return;
}

gorc::sexpr gorc::make_sexpr()
{
    return std::make_shared<sexpr_node>();
}

gorc::sexpr gorc::make_sexpr(sexpr left, sexpr right)
{
    return std::make_shared<sexpr_node>(left, right);
}

bool gorc::atom(sexpr e)
{
    return e->type == sexpr_node_type::atom;
}

bool gorc::null(sexpr e)
{
    return atom(e) && e->atom.empty();
}

gorc::sexpr gorc::car(sexpr e)
{
    if(atom(e)) {
        LOG_FATAL("expected data structure, found atom");
    }

    return e->left;
}

gorc::sexpr gorc::cdr(sexpr e)
{
    if(atom(e)) {
        LOG_FATAL("expected data structure, found atom");
    }

    return e->right;
}
