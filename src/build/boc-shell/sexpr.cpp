#include "sexpr.hpp"
#include "log/log.hpp"

gorc::sexpr_int_type::sexpr_int_type(sexpr left, sexpr right)
    : left(left)
    , right(right)
{
    return;
}

gorc::sexpr_leaf_type::sexpr_leaf_type(std::string const &value)
    : value(value)
{
    return;
}

gorc::sexpr_leaf_type::operator bool() const
{
    if(value == "true") {
        return true;
    }
    else if(value == "false") {
        return false;
    }

    LOG_FATAL(format("value '%s' is not boolean") % value);
}

gorc::sexpr_leaf_type::operator std::string() const
{
    return value;
}

gorc::sexpr gorc::make_sexpr(sexpr left, sexpr right)
{
    return sexpr(sexpr_node(std::make_shared<sexpr_int_type>(left, right)));
}
