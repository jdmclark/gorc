#include "builtins.hpp"
#include "sexpr/sexpr.hpp"
#include "sexpr/sexpr_helpers.hpp"
#include "symbols.hpp"
#include "stack.hpp"
#include "io/path.hpp"
#include <boost/filesystem.hpp>

using arglist = std::vector<gorc::sexpr>;

void gorc::register_builtins()
{
    // Built-in global variables

    create_variable("?", make_sexpr()); // Result code

    // Built-in functions

    register_builtin("atom", 1, [](arglist const &args)
            {
                return make_sexpr(atom(args.at(0)));
            });

    register_builtin("car", 1, [](arglist const &args)
            {
                return car(args.at(0));
            });

    register_builtin("cdr", 1, [](arglist const &args)
            {
                return cdr(args.at(0));
            });

    register_builtin("null", 1, [](arglist const &args)
            {
                return make_sexpr(null(args.at(0)));
            });

    register_builtin("is_file", 1, [](arglist const &args)
            {
                path top_arg = as_string_value(args.at(0));
                return make_sexpr(boost::filesystem::is_regular_file(top_arg));
            });
}
