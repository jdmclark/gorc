#include "io_redirection_visitor.hpp"
#include "argument_visitor.hpp"
#include "utility/make_unique.hpp"
#include "value.hpp"
#include "log/log.hpp"

gorc::io_redirection_visitor::io_redirection_visitor(bool input)
    : input(input)
{
    return;
}

std::unique_ptr<gorc::pipe> gorc::io_redirection_visitor::visit(pipe_io_redirection &target) const
{
    auto target_file_vec = ast_visit(argument_visitor(), target.target);
    if(target_file_vec.size() != 1) {
        LOG_FATAL("redirection target is not a filename");
    }

    auto target_file = target_file_vec.front();

    if(input) {
        return make_unique<pipe>(make_input_file_pipe(target_file));
    }
    else if(target.append) {
        return make_unique<pipe>(make_output_file_append_pipe(target_file));
    }
    else {
        return make_unique<pipe>(make_output_file_pipe(target_file));
    }
}
