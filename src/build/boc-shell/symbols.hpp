#pragma once

#include "ast.hpp"
#include "sexpr/sexpr.hpp"
#include "utility/make_unique.hpp"
#include <functional>
#include <vector>

namespace gorc {

    class builtin {
    public:
        size_t args;
        std::function<sexpr(std::vector<sexpr> const &)> code;

        builtin(size_t args, std::function<sexpr(std::vector<sexpr> const &)> code);
    };

    void register_builtin(std::string const &name,
                          std::unique_ptr<builtin> &&obj);

    template <typename FunctorT>
    void register_builtin(std::string const &name,
                          size_t args,
                          FunctorT fn)
    {
        register_builtin(name, make_unique<builtin>(args, fn));
    }

    maybe<builtin*> get_builtin(std::string const &name);

    void register_function(func_declaration_statement &);
    func_declaration_statement& get_function(std::string const &name);
}
