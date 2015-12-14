#pragma once

#include "ast.hpp"
#include "system/pipe.hpp"

namespace gorc {

    class io_redirection_visitor {
    private:
        bool input;

    public:
        io_redirection_visitor(bool input);

        std::unique_ptr<pipe> visit(pipe_io_redirection &) const;
    };

}
