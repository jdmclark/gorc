#include "argument_visitor.hpp"
#include "word_visitor.hpp"
#include "utility/range.hpp"
#include "log/log.hpp"

gorc::sexpr gorc::argument_visitor::visit(argument &arg) const
{
    std::vector<sexpr> arg_parts;
    for(auto &part : arg.words->elements) {
        arg_parts.push_back(ast_visit(word_visitor(), *part));
    }

    if(arg_parts.size() == 1) {
        return arg_parts.front();
    }

    auto assembled_arg = make_sexpr();
    for(auto &part : make_reverse_range(arg_parts)) {
        assembled_arg = make_sexpr(part, assembled_arg);
    }

    return assembled_arg;
}

gorc::sexpr gorc::argument_visitor::visit(ast_list_node<argument*> &args) const
{
    std::vector<sexpr> arg_list;
    for(auto &arg : args.elements) {
        arg_list.push_back(ast_visit(*this, arg));
    }

    if(arg_list.size() == 1) {
        return arg_list.front();
    }

    auto assembled_args = make_sexpr();
    for(auto &arg : make_reverse_range(arg_list)) {
        assembled_args = make_sexpr(arg, assembled_args);
    }

    return assembled_args;
}
