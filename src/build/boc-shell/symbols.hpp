#pragma once

#include "ast.hpp"

namespace gorc {

    void register_function(func_declaration_statement &);
    func_declaration_statement& get_function(std::string const &name);

}
