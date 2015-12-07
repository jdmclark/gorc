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

    auto assembled_args = make_sexpr();
    for(auto &arg : make_reverse_range(arg_list)) {
        assembled_args = make_sexpr(arg, assembled_args);
    }

    return assembled_args;
}

namespace {

    void inner_word_seq(std::vector<std::string> &out,
                        std::string &accumulator,
                        gorc::sexpr e)
    {
        if(null(e)) {
            return;
        }
        else if(atom(e)) {
            LOG_FATAL("invalid argument list: found atom, expected word list");
        }

        if(!accumulator.empty()) {
            out.push_back(accumulator);
            accumulator.clear();
        }

        if(!atom(car(e))) {
            LOG_FATAL("invalid argument list: expected word, found list");
        }

        accumulator = car(e)->atom;
        inner_word_seq(out, accumulator, cdr(e));
    }

    void inner_word_to_arg_fragment(std::vector<std::string> &out,
                                    std::string &accumulator,
                                    gorc::sexpr e)
    {
        if(null(e)) {
            return;
        }
        else if(atom(e)) {
            accumulator += e->atom;
            return;
        }

        // Each word is either a bareword or a list
        gorc::sexpr head = car(e);
        if(atom(head)) {
            accumulator += head->atom;
        }
        else if(!atom(car(head))) {
            // Head is a list, but the first element is not a word
            LOG_FATAL("invalid argument list: found list, expected word");
        }
        else {
            accumulator += car(head)->atom;
            inner_word_seq(out, accumulator, cdr(head));
        }

        inner_word_to_arg_fragment(out, accumulator, cdr(e));
    }

    void inner_arg_list_to_argv(std::vector<std::string> &out, gorc::sexpr e)
    {
        if(null(e)) {
            return;
        }
        else if(atom(e)) {
            LOG_FATAL("invalid argument list: found atom, expected argument list");
        }

        // Each argument is a list of words. These words are catted together.
        std::string accumulator;
        inner_word_to_arg_fragment(out, accumulator, car(e));

        if(!accumulator.empty()) {
            out.push_back(accumulator);
        }

        inner_arg_list_to_argv(out, cdr(e));
    }

}

std::vector<std::string> gorc::argument_list_to_argv(sexpr e)
{
    std::vector<std::string> out;
    inner_arg_list_to_argv(out, e);
    return out;
}
