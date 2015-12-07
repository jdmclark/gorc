#include "sexpr_helpers.hpp"
#include "log/log.hpp"

namespace {

    class sexpr_as_string_visitor {
    public:
        std::string visit(gorc::sexpr_leaf const &n) const
        {
            return static_cast<std::string>(*n);
        }

        std::string visit(gorc::sexpr_int const &) const
        {
            LOG_FATAL("expected string, found data structure");
        }
    };

}

std::string gorc::as_string_value(sexpr s)
{
    if(s.has_value()) {
        return s.get_value().accept(sexpr_as_string_visitor());
    }

    LOG_FATAL("expected string, found nil");
}
