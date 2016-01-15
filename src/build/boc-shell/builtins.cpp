#include "builtins.hpp"
#include "symbols.hpp"
#include "stack.hpp"
#include "io/path.hpp"
#include <boost/filesystem.hpp>

void gorc::register_builtins()
{
    // Built-in global variables

    create_variable("?", shvalue()); // Result code

    // Built-in functions

    register_builtin("null", 1, [](arglist const &args)
            {
                return shvalue_from_boolean(args.at(0).empty());
            });

    register_builtin("head", 1, [](arglist const &args)
            {
                auto const &list = args.at(0);
                if(list.empty()) {
                    return shvalue();
                }
                else {
                    return shvalue({ list.front() });
                }
            });

    register_builtin("tail", 1, [](arglist const &args)
            {
                auto const &list = args.at(0);
                if(list.empty()) {
                    return shvalue();
                }

                shvalue rv;
                auto it = list.begin();
                ++it;

                std::copy(it, list.end(), std::back_inserter(rv));
                return rv;
            });

    register_builtin("is_file", 1, [](arglist const &args)
            {
                path top_arg = shvalue_to_string(args.at(0));
                return shvalue_from_boolean(boost::filesystem::is_regular_file(top_arg));
            });

    register_builtin("absolute_path", 1, [](arglist const &args)
            {
                path top_arg = shvalue_to_string(args.at(0));
                return shvalue_from_string(boost::filesystem::absolute(top_arg).generic_string());
            });

    register_builtin("canonical_path", 1, [](arglist const &args)
            {
                path top_arg = shvalue_to_string(args.at(0));
                return shvalue_from_string(boost::filesystem::canonical(top_arg).generic_string());
            });
}
