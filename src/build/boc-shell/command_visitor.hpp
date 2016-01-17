#pragma once

#include "ast.hpp"
#include "value.hpp"
#include "utility/maybe.hpp"
#include <vector>

namespace gorc {

    class command_visitor {
    private:
        maybe<shvalue*> stdout_variable_target;

    public:
        command_visitor(maybe<shvalue*> stdout_variable_target = nothing);

        shvalue exit_code_sequence;

        int visit(pipe_command &);
        int visit(infix_command &);
    };

}
