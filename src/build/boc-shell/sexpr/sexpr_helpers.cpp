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

namespace {
    std::vector<std::string> argument_part_to_argv(gorc::sexpr e)
    {
        if(null(e)) {
            return std::vector<std::string>();
        }
        else if(atom(e)) {
            return std::vector<std::string> { e->atom };
        }
        else {
            std::vector<std::string> left = argument_part_to_argv(car(e));
            std::vector<std::string> right = argument_part_to_argv(cdr(e));

            std::copy(right.begin(), right.end(), std::back_inserter(left));
            return left;
        }
    }
}

std::vector<std::string> gorc::argument_to_argv(sexpr e)
{
    if(null(e)) {
        return std::vector<std::string>();
    }
    else if(atom(e)) {
        return std::vector<std::string> { e->atom };
    }
    else {
        std::vector<std::string> left = argument_part_to_argv(car(e));
        std::vector<std::string> right = argument_to_argv(cdr(e));

        if(!right.empty()) {
            auto it = right.begin();
            left.back() += *it;

            ++it;

            for( ; it != right.end(); ++it) {
                left.push_back(*it);
            }
        }

        return left;
    }
}

std::vector<std::string> gorc::argument_list_to_argv(sexpr e)
{
    std::vector<std::string> rv;

    auto current = e;
    while(!null(current)) {
        auto tv = argument_to_argv(car(current));
        std::copy(tv.begin(), tv.end(), std::back_inserter(rv));
        current = cdr(current);
    }

    return rv;
}
