#include "sexpr_helpers.hpp"
#include "log/log.hpp"

std::string gorc::as_string_value(sexpr s)
{
    if(atom(s)) {
        return s->atom;
    }

    LOG_FATAL("expected string, found data structure");
}

bool gorc::as_boolean_value(sexpr s)
{
    if(atom(s)) {
        if(s->atom == "true") {
            return true;
        }
        else if(s->atom == "false") {
            return false;
        }
        else {
            LOG_FATAL(format("value '%s' is not boolean") % s->atom);
        }
    }

    LOG_FATAL("expected boolean, found data structure");
}

bool gorc::sexpr_equal(sexpr left, sexpr right)
{
    if(atom(left) != atom(right)) {
        return false;
    }
    else if(atom(left)) {
        return left->atom == right->atom;
    }
    else {
        return sexpr_equal(left->left, right->left) &&
               sexpr_equal(left->right, right->right);
    }
}
