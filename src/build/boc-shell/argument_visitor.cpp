#include "argument_visitor.hpp"
#include "word_visitor.hpp"

std::string gorc::argument_visitor::visit(argument &arg) const
{
    std::string assembled_arg;
    for(auto &part : arg.words->elements) {
        assembled_arg += ast_visit(word_visitor(), *part);
    }

    return assembled_arg;
}
