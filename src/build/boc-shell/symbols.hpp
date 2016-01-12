#pragma once

#include "ast.hpp"
#include "value.hpp"
#include <memory>
#include <functional>
#include <vector>

namespace gorc {

    using arglist = std::vector<shvalue>;

    class builtin {
    public:
        size_t args;
        std::function<shvalue(arglist const &)> code;

        builtin(size_t args, std::function<shvalue(arglist const &)> code);
    };

    void register_builtin(std::string const &name,
                          std::unique_ptr<builtin> &&obj);

    template <typename FunctorT>
    void register_builtin(std::string const &name,
                          size_t args,
                          FunctorT fn)
    {
        register_builtin(name, std::make_unique<builtin>(args, fn));
    }

    maybe<builtin*> get_builtin(std::string const &name);

    void register_function(func_declaration_statement &);
    func_declaration_statement& get_function(std::string const &name);
}
